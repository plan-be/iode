import pytest
import pytestqt

from pathlib import Path
from typing import List

from qtpy.QtCore import Qt, QDir, QItemSelection, QItemSelectionModel, QMimeData, QModelIndex, QPointF
from qtpy.QtGui import QDropEvent
from qtpy.QtWidgets import QMessageBox, QApplication, QLineEdit, QAbstractItemView

from iode import variables, VarsMode
from iode_gui.iode_objs.models.table_model import VariablesModel
from iode_gui.iode_objs.views.table_view import VariablesView
from iode_gui.main_widgets.file_explorer import file_explorer as file_explorer_module
from iode_gui.main_widgets.file_explorer.file_explorer import IodeFileExplorer


# ==== Variables View ====

@pytest.fixture
def variables_view(qtbot):
    # small, known Variables workspace
    variables.clear()
    variables.sample = "2000Y1:2005Y1"
    variables["A"] = 0.0
    variables["B"] = 0.0

    view = VariablesView()
    qtbot.addWidget(view)
    model = VariablesModel(precision=2, format='f', mode=VarsMode.LEVEL, parent=view)
    view.setModel(model)

    yield view

    variables.clear()


def test_variables_view_paste_special(variables_view, monkeypatch):
    view: VariablesView = variables_view

    # record warnings instead of opening a blocking message box
    warnings = []
    monkeypatch.setattr(QMessageBox, "warning", lambda *args, **kwargs: warnings.append(args))

    # the sample dialog must not be opened when the columns represent periods
    def fail_ask_sample():
        raise AssertionError("_ask_sample() should not have been called")
    monkeypatch.setattr(view, "_ask_sample", fail_ask_sample)

    # ---- columns = contiguous periods ----
    view._paste_special(["A", "B"], ["2001Y1", "2002Y1", "2003Y1"], [[1.0, 2.0, 3.0], [4.0, 5.0, 6.0]])
    assert [variables["A", p] for p in ["2000Y1", "2001Y1", "2002Y1", "2003Y1", "2004Y1"]] == [0.0, 1.0, 2.0, 3.0, 0.0]
    assert [variables["B", p] for p in ["2000Y1", "2001Y1", "2002Y1", "2003Y1", "2004Y1"]] == [0.0, 4.0, 5.0, 6.0, 0.0]

    # ---- columns = integers (yearly periods) + surrounding spaces ----
    view._paste_special(["A", "B"], [" 2004", "2005 "], [[7.0, 8.0], [9.0, 10.0]])
    assert [variables["A", p] for p in ["2004Y1", "2005Y1"]] == [7.0, 8.0]
    assert [variables["B", p] for p in ["2004Y1", "2005Y1"]] == [9.0, 10.0]

    # ---- columns = non-contiguous periods ----
    view._paste_special(["A", "B"], ["2000", "2002Y1"], [[11.0, 12.0], [13.0, 14.0]])
    assert [variables["A", p] for p in ["2000Y1", "2001Y1", "2002Y1"]] == [11.0, 1.0, 12.0]
    assert [variables["B", p] for p in ["2000Y1", "2001Y1", "2002Y1"]] == [13.0, 4.0, 14.0]
    assert not warnings

    # ---- columns are not periods -> the user is asked for the sample ----
    monkeypatch.setattr(view, "_ask_sample", lambda: ("2001Y1", "2003Y1"))
    view._paste_special(["A", "B"], ["1.5", "2.5", "3.5"], [[21.0, 22.0, 23.0], [24.0, 25.0, 26.0]])
    assert [variables["A", p] for p in ["2001Y1", "2002Y1", "2003Y1"]] == [21.0, 22.0, 23.0]
    assert [variables["B", p] for p in ["2001Y1", "2002Y1", "2003Y1"]] == [24.0, 25.0, 26.0]
    assert not warnings

    # ---- columns are not periods + the user cancels the sample dialog -> nothing pasted ----
    monkeypatch.setattr(view, "_ask_sample", lambda: None)
    view._paste_special(["A"], ["1.5", "2.5", "3.5"], [[31.0, 32.0, 33.0]])
    assert [variables["A", p] for p in ["2001Y1", "2002Y1", "2003Y1"]] == [21.0, 22.0, 23.0]
    assert not warnings

    # ---- columns are not periods + sample size != number of values -> warning, nothing pasted ----
    monkeypatch.setattr(view, "_ask_sample", lambda: ("2001Y1", "2002Y1"))
    view._paste_special(["A"], ["1.5", "2.5", "3.5"], [[41.0, 42.0, 43.0]])
    assert len(warnings) == 1
    assert "2001Y1:2002Y1" in warnings[0][2]
    assert [variables["A", p] for p in ["2001Y1", "2002Y1", "2003Y1"]] == [21.0, 22.0, 23.0]


def test_variables_view_paste(variables_view, qtbot, monkeypatch):
    view: VariablesView = variables_view
    model: VariablesModel = view.model()

    warnings = []
    monkeypatch.setattr(QMessageBox, "warning", lambda *args, **kwargs: warnings.append(args))

    # select the block A[2001Y1:2002Y1] - B[2001Y1:2002Y1] (paste starts from the top-left cell)
    selection = QItemSelection(model.index(0, 1), model.index(1, 2))
    view.selectionModel().select(selection, QItemSelectionModel.SelectionFlag.Select)

    # ---- values copied from Excel (tab-separated, '\r\n' line endings) ----
    # NOTE: the first value is equal to the current value (0) -> the paste must not stop
    QApplication.clipboard().setText("0\t2\r\n3\t4\r\n")
    with qtbot.waitSignal(model.dataChanged, timeout=1000):
        view.paste()
    assert not warnings
    assert [variables["A", p] for p in ["2000Y1", "2001Y1", "2002Y1", "2003Y1"]] == [0.0, 0.0, 2.0, 0.0]
    assert [variables["B", p] for p in ["2000Y1", "2001Y1", "2002Y1", "2003Y1"]] == [0.0, 3.0, 4.0, 0.0]
    assert model.data(model.index(1, 2), Qt.ItemDataRole.DisplayRole) == "4.00"

    # ---- more values than available columns -> warning ----
    QApplication.clipboard().setText("\t".join(["1"] * 10))
    view.paste()
    assert len(warnings) == 1
    assert "Invalid index" in warnings[0][2]


# ==== File Explorer ====

def generate_dirs_and_files(data_dir: Path) -> List[Path]:
    """
    Create a random number of sub-folders, sub-sub-folders and files 
    inside 'test_delete'.

    Returns the list of created folders and files.
    """
    test_dir = data_dir / 'test_delete'

    # create the sub-folder 'test_delete' if it does not exists
    test_dir.mkdir(parents=True, exist_ok=True)

    # create sub-folders, sub-sub-folders and files inside 'test_delete'
    created_paths: List[Path] = []
    for i in range(1, 5):
        sub_dir = test_dir / f"sub_folder_{i}"
        sub_dir.mkdir(exist_ok=True)
        created_paths.append(sub_dir)
        for k in range(3):
            file = sub_dir / f"file_{i}_{k}.txt"
            file.write_text(f"content of file {i}_{k}")
            created_paths.append(file)
        for j in range(0, 3):
            sub_sub_dir = sub_dir / f"sub_sub_folder_{j}"
            sub_sub_dir.mkdir(exist_ok=True)
            created_paths.append(sub_sub_dir)
            for k in range(3):
                file = sub_sub_dir / f"file_{i}_{j}_{k}.txt"
                file.write_text(f"content of file {i}_{j}_{k}")
                created_paths.append(file)
    return created_paths


class _FakeTabWidget:
    """Replaces the IodeTabWidget (not needed to create, move or delete files)"""
    def update_project_dir(self, project_dir_path: str):
        pass


def get_index(explorer: IodeFileExplorer, path: Path) -> QModelIndex:
    return explorer.proxy_model.index_from_path(str(path))


def wait_until_visible(qtbot, explorer: IodeFileExplorer, path: Path):
    """Wait until the File Explorer has loaded the given file or directory"""
    # QFileSystemModel loads the content of directories asynchronously
    # -> expand the parent directories so that their content is loaded
    for parent in reversed(path.parents):
        index = get_index(explorer, parent)
        if index.isValid():
            explorer.expand(index)
    qtbot.waitUntil(lambda: get_index(explorer, path).isValid(), timeout=5000)


def select(explorer: IodeFileExplorer, paths: List[Path]):
    """Select files and/or directories in the File Explorer (as a user would do with the mouse)"""
    selection_model = explorer.selectionModel()
    selection_model.clearSelection()
    for path in paths:
        index = get_index(explorer, path)
        assert index.isValid(), f"'{path}' not found in the File Explorer"
        selection_model.select(index, QItemSelectionModel.SelectionFlag.Select)


@pytest.fixture
def file_explorer(qtbot, tmp_path, monkeypatch):
    # create folders and files in tmp_path/test_delete
    created_paths = generate_dirs_and_files(tmp_path)
    test_dir = tmp_path / "test_delete"
    assert sorted(test_dir.rglob("*")) == sorted(created_paths)

    # do not read/write the (user or project) settings
    monkeypatch.setattr(file_explorer_module, "get_settings", lambda: None)

    explorer = IodeFileExplorer()
    qtbot.addWidget(explorer)
    explorer.tab_widget = _FakeTabWidget()
    # project directory = tmp_path
    explorer.update_project_dir(QDir(str(tmp_path)), on_startup=True)

    for i in range(1, 5):
        wait_until_visible(qtbot, explorer, test_dir / f"sub_folder_{i}" / f"sub_sub_folder_0" / f"file_{i}_0_0.txt")

    yield explorer, test_dir


def test_file_explorer_create_file_and_dir(file_explorer):
    explorer, test_dir = file_explorer
    sub_dir = test_dir / "sub_folder_1"

    # ---- a directory is selected -> new file/folder created inside it ----
    select(explorer, [sub_dir])
    explorer.create_file()
    assert (sub_dir / "new_file").is_file()

    select(explorer, [sub_dir])
    explorer.create_dir()
    assert (sub_dir / "new_folder").is_dir()

    # ---- a file is selected -> new file/folder created in the parent directory of the file ----
    sub_sub_dir = sub_dir / "sub_sub_folder_0"
    select(explorer, [sub_sub_dir / "file_1_0_0.txt"])
    explorer.create_file()
    assert (sub_sub_dir / "new_file").is_file()

    select(explorer, [sub_sub_dir / "file_1_0_0.txt"])
    explorer.create_dir()
    assert (sub_sub_dir / "new_folder").is_dir()

    # ---- nothing selected -> new file/folder created in the project directory ----
    explorer.selectionModel().clearSelection()
    explorer.create_file()
    explorer.create_dir()
    project_dir = test_dir.parent
    assert (project_dir / "new_file").is_file()
    assert (project_dir / "new_folder").is_dir()


def test_file_explorer_move_files_and_dirs(file_explorer, qtbot, monkeypatch):
    explorer, test_dir = file_explorer
    sub_dir_1 = test_dir / "sub_folder_1"
    sub_dir_2 = test_dir / "sub_folder_2"
    sub_dir_3 = test_dir / "sub_folder_3"

    # record the 'overwrite ?' questions
    questions = []
    question_answer = QMessageBox.StandardButton.Yes
    def question(*args, **kwargs):
        questions.append(args)
        return question_answer
    monkeypatch.setattr(QMessageBox, "question", question)

    # an error while moving is reported in a warning box -> record it instead of blocking the test
    warnings = []
    monkeypatch.setattr(QMessageBox, "warning", lambda *args, **kwargs: warnings.append(args))

    # ---- cut/paste a file and a directory (with its content) into another directory ----
    # NOTE: the destination directory 'test_delete' does not contain a file or
    #       directory with the same name -> no overwrite question
    src_file = sub_dir_1 / "file_1_0.txt"
    src_dir = sub_dir_1 / "sub_sub_folder_0"
    select(explorer, [src_file, src_dir])
    explorer.cut()
    select(explorer, [test_dir])
    with qtbot.waitSignals([explorer.fileMoved, explorer.fileMoved], timeout=1000):
        explorer.paste()
    assert not warnings
    assert not questions
    assert not src_file.exists()
    assert not src_dir.exists()
    assert (test_dir / "file_1_0.txt").read_text() == "content of file 1_0"
    assert sorted(p.name for p in (test_dir / "sub_sub_folder_0").iterdir()) == \
        ["file_1_0_0.txt", "file_1_0_1.txt", "file_1_0_2.txt"]

    # ---- cut/paste a file onto an existing file -> overwrite (Yes) ----
    (sub_dir_2 / "file_3_0.txt").write_text("old content")
    src_file = sub_dir_3 / "file_3_0.txt"
    select(explorer, [src_file])
    explorer.cut()
    select(explorer, [sub_dir_2 / "file_2_0.txt"])     # file selected -> paste in its parent directory
    explorer.paste()
    assert len(questions) == 1
    assert not src_file.exists()
    assert (sub_dir_2 / "file_3_0.txt").read_text() == "content of file 3_0"

    # ---- cut/paste a file onto an existing file -> do not overwrite (No) -> suffix '_copy' ----
    questions.clear()
    question_answer = QMessageBox.StandardButton.No
    (sub_dir_2 / "file_3_1.txt").write_text("old content")
    src_file = sub_dir_3 / "file_3_1.txt"
    select(explorer, [src_file])
    explorer.cut()
    select(explorer, [sub_dir_2])
    explorer.paste()
    assert len(questions) == 1
    assert not src_file.exists()
    assert (sub_dir_2 / "file_3_1.txt").read_text() == "old content"
    assert (sub_dir_2 / "file_3_1_copy.txt").read_text() == "content of file 3_1"

    # ---- cut/paste a directory onto an existing non-empty directory -> overwrite (Yes) ----
    # -> the content of both directories is merged (files with the same name are overwritten)
    questions.clear()
    question_answer = QMessageBox.StandardButton.Yes
    src_dir = sub_dir_2 / "sub_sub_folder_0"
    dst_dir = sub_dir_3 / "sub_sub_folder_0"
    (dst_dir / "file_2_0_0.txt").write_text("old content")
    select(explorer, [src_dir])
    explorer.cut()
    select(explorer, [sub_dir_3])
    with qtbot.waitSignal(explorer.fileMoved, timeout=1000) as blocker:
        explorer.paste()
    assert blocker.args == [str(src_dir), str(dst_dir)]
    assert not warnings
    assert len(questions) == 1
    assert not src_dir.exists()
    assert sorted(p.name for p in dst_dir.iterdir()) == \
        ["file_2_0_0.txt", "file_2_0_1.txt", "file_2_0_2.txt",
         "file_3_0_0.txt", "file_3_0_1.txt", "file_3_0_2.txt"]
    assert (dst_dir / "file_2_0_0.txt").read_text() == "content of file 2_0_0"
    assert (dst_dir / "file_3_0_0.txt").read_text() == "content of file 3_0_0"

    # ---- cut/paste a file into its own directory -> nothing happens (the file is not lost) ----
    questions.clear()
    src_file = sub_dir_2 / "file_2_1.txt"
    select(explorer, [src_file])
    explorer.cut()
    select(explorer, [sub_dir_2])
    explorer.paste()
    assert not warnings
    assert src_file.read_text() == "content of file 2_1"

    # ---- cut/paste a directory into one of its own sub-directories -> error, nothing moved ----
    select(explorer, [sub_dir_2])
    explorer.cut()
    wait_until_visible(qtbot, explorer, sub_dir_2 / "sub_sub_folder_1")
    select(explorer, [sub_dir_2 / "sub_sub_folder_1"])
    explorer.paste()
    assert len(warnings) == 1
    assert "into one of its own sub-directories" in warnings[0][2]
    assert (sub_dir_2 / "sub_sub_folder_1" / "file_2_1_0.txt").exists()
    warnings.clear()

    # ---- cut/paste a file onto an existing directory with the same name -> error, nothing moved ----
    questions.clear()
    sub_dir_4 = test_dir / "sub_folder_4"
    tmp_file = test_dir / "sub_sub_folder_1"
    tmp_file.write_text("a file with the name of a directory")
    wait_until_visible(qtbot, explorer, tmp_file)
    select(explorer, [tmp_file])
    explorer.cut()
    select(explorer, [sub_dir_4])
    explorer.paste()
    # error reported before asking to overwrite
    assert not questions
    assert len(warnings) == 1
    assert "Cannot replace the directory" in warnings[0][2]
    assert tmp_file.read_text() == "a file with the name of a directory"
    assert (sub_dir_4 / "sub_sub_folder_1" / "file_4_1_0.txt").exists()
    warnings.clear()

    # ---- copy/paste a file onto an existing directory with the same name -> error, nothing copied ----
    select(explorer, [tmp_file])
    explorer.copy()
    select(explorer, [sub_dir_4])
    explorer.paste()
    assert not questions
    assert len(warnings) == 1
    assert "Cannot replace the directory" in warnings[0][2]
    assert tmp_file.read_text() == "a file with the name of a directory"
    assert not (sub_dir_4 / "sub_sub_folder_1" / "sub_sub_folder_1").exists()
    assert sorted(p.name for p in (sub_dir_4 / "sub_sub_folder_1").iterdir()) == \
        ["file_4_1_0.txt", "file_4_1_1.txt", "file_4_1_2.txt"]
    warnings.clear()

    # ---- cut/paste a directory onto an existing file with the same name -> error, nothing moved ----
    questions.clear()
    (sub_dir_4 / "sub_sub_folder_2").rename(sub_dir_4 / "renamed_folder")
    (sub_dir_4 / "sub_sub_folder_2").write_text("a file with the name of a directory")
    src_dir = sub_dir_1 / "sub_sub_folder_2"
    select(explorer, [src_dir])
    explorer.cut()
    select(explorer, [sub_dir_4])
    explorer.paste()
    # error reported before asking to overwrite
    assert not questions
    assert len(warnings) == 1
    assert "Cannot replace the file" in warnings[0][2]
    assert (src_dir / "file_1_2_0.txt").exists()
    assert (sub_dir_4 / "sub_sub_folder_2").read_text() == "a file with the name of a directory"
    warnings.clear()

    # ---- copy/paste a directory onto an existing file with the same name -> error, nothing copied ----
    select(explorer, [src_dir])
    explorer.copy()
    select(explorer, [sub_dir_4])
    explorer.paste()
    assert not questions
    assert len(warnings) == 1
    assert "Cannot replace the file" in warnings[0][2]
    assert (src_dir / "file_1_2_0.txt").exists()
    assert (sub_dir_4 / "sub_sub_folder_2").read_text() == "a file with the name of a directory"
    warnings.clear()

    # ---- drag and drop a file onto a directory ----
    questions.clear()
    src_file = sub_dir_3 / "file_3_2.txt"
    explorer.show()
    dst_index = get_index(explorer, sub_dir_1)
    explorer.scrollTo(dst_index)
    drop_position = QPointF(explorer.visualRect(dst_index).center())
    mime_data = QMimeData()
    mime_data.setText(f"file:///{src_file.as_posix()}")
    drop_event = QDropEvent(drop_position, Qt.DropAction.MoveAction, mime_data,
                            Qt.MouseButton.LeftButton, Qt.KeyboardModifier.NoModifier)
    with qtbot.waitSignal(explorer.fileMoved, timeout=1000) as blocker:
        explorer.dropEvent(drop_event)
    assert blocker.args == [str(src_file), str(sub_dir_1 / "file_3_2.txt")]
    assert not questions
    assert not src_file.exists()
    assert (sub_dir_1 / "file_3_2.txt").read_text() == "content of file 3_2"
    assert not warnings


def test_file_explorer_delete_files_and_dirs(file_explorer, monkeypatch):
    explorer, test_dir = file_explorer
    sub_dir_1 = test_dir / "sub_folder_1"

    # record the 'Are you sure ?' messages
    messages = []
    answer = QMessageBox.StandardButton.Yes
    def warning(parent, title, text, *args, **kwargs):
        messages.append(text)
        return answer
    monkeypatch.setattr(QMessageBox, "warning", warning)

    # ---- the user answers No -> nothing deleted ----
    answer = QMessageBox.StandardButton.No
    select(explorer, [sub_dir_1 / "file_1_0.txt"])
    explorer.remove()
    assert messages[-1] == "Are you sure to delete the file 'file_1_0.txt' ?"
    assert (sub_dir_1 / "file_1_0.txt").exists()

    answer = QMessageBox.StandardButton.Yes

    # ---- delete one file ----
    select(explorer, [sub_dir_1 / "file_1_0.txt"])
    explorer.remove()
    assert not (sub_dir_1 / "file_1_0.txt").exists()
    assert (sub_dir_1 / "file_1_1.txt").exists()

    # ---- delete one directory and its content ----
    select(explorer, [sub_dir_1 / "sub_sub_folder_0"])
    explorer.remove()
    assert messages[-1] == "Are you sure to delete the directory 'sub_sub_folder_0' and its content ?"
    assert not (sub_dir_1 / "sub_sub_folder_0").exists()
    assert (sub_dir_1 / "sub_sub_folder_1").exists()

    # ---- delete several files and directories at once ----
    to_delete = [sub_dir_1 / "file_1_1.txt", sub_dir_1 / "sub_sub_folder_1", test_dir / "sub_folder_2"]
    select(explorer, to_delete)
    explorer.remove()
    assert "the following 3 files/directories and their content" in messages[-1]
    assert not any(path.exists() for path in to_delete)
    assert (sub_dir_1 / "file_1_2.txt").exists()
    assert (sub_dir_1 / "sub_sub_folder_2").exists()

    # ---- delete the whole 'test_delete' directory ----
    select(explorer, [test_dir])
    explorer.remove()
    assert not test_dir.exists()
    assert test_dir.parent.exists()


def rename_with_editor(qtbot, explorer: IodeFileExplorer, path: Path, new_name: str,
                       key: Qt.Key = Qt.Key.Key_Return):
    """Rename a file or directory as a user would do (F2 -> type the new name -> press ENTER)"""
    select(explorer, [path])
    explorer.rename()
    # NOTE: editors of previous renamings are hidden but may not be deleted yet
    editors = [editor for editor in explorer.findChildren(QLineEdit) if editor.isVisible()]
    assert len(editors) == 1
    editor: QLineEdit = editors[0]
    # the editor is initialized with the current name
    assert editor.text() == path.name
    editor.setText(new_name)
    qtbot.keyClick(editor, key)
    # wait until the editor is closed
    qtbot.waitUntil(lambda: explorer.state() != QAbstractItemView.State.EditingState, timeout=1000)


def test_file_explorer_rename_files_and_dirs(file_explorer, qtbot, monkeypatch):
    explorer, test_dir = file_explorer
    sub_dir_1 = test_dir / "sub_folder_1"
    sub_dir_2 = test_dir / "sub_folder_2"

    # record the 'overwrite ?' questions
    questions = []
    question_answer = QMessageBox.StandardButton.Yes
    def question(*args, **kwargs):
        questions.append(args)
        return question_answer
    monkeypatch.setattr(QMessageBox, "question", question)

    # an error while renaming is reported in a warning box -> record it instead of blocking the test
    warnings = []
    monkeypatch.setattr(QMessageBox, "warning", lambda *args, **kwargs: warnings.append(args))

    # the editor (QLineEdit) is only created if the File Explorer is visible
    explorer.show()

    # ---- rename a file ----
    # the File Explorer is notified (fileMoved) to update the tab associated with the file if open
    src_file = sub_dir_1 / "file_1_0.txt"
    with qtbot.waitSignal(explorer.fileMoved, timeout=1000) as blocker:
        rename_with_editor(qtbot, explorer, src_file, "renamed_file.txt")
    assert blocker.args == [str(src_file), str(sub_dir_1 / "renamed_file.txt")]
    assert not src_file.exists()
    assert (sub_dir_1 / "renamed_file.txt").read_text() == "content of file 1_0"
    assert not questions
    assert not warnings

    # ---- rename a directory (its content is kept) ----
    src_dir = sub_dir_1 / "sub_sub_folder_0"
    with qtbot.waitSignal(explorer.fileMoved, timeout=1000) as blocker:
        rename_with_editor(qtbot, explorer, src_dir, "renamed_folder")
    assert blocker.args == [str(src_dir), str(sub_dir_1 / "renamed_folder")]
    assert not src_dir.exists()
    assert sorted(p.name for p in (sub_dir_1 / "renamed_folder").iterdir()) == \
        ["file_1_0_0.txt", "file_1_0_1.txt", "file_1_0_2.txt"]
    assert not questions
    assert not warnings

    # ---- press ESCAPE -> renaming cancelled ----
    src_file = sub_dir_1 / "file_1_1.txt"
    rename_with_editor(qtbot, explorer, src_file, "cancelled.txt", key=Qt.Key.Key_Escape)
    assert src_file.read_text() == "content of file 1_1"
    assert not (sub_dir_1 / "cancelled.txt").exists()

    # ---- same name -> nothing changes (no 'overwrite ?' question) ----
    rename_with_editor(qtbot, explorer, src_file, src_file.name)
    assert src_file.read_text() == "content of file 1_1"
    assert not questions
    assert not warnings

    # ---- empty name -> nothing changes ----
    rename_with_editor(qtbot, explorer, src_file, "")
    assert src_file.read_text() == "content of file 1_1"
    assert not questions
    assert not warnings

    # ---- rename a file with the name of an existing file -> overwrite (Yes) ----
    src_file = sub_dir_2 / "file_2_0.txt"
    rename_with_editor(qtbot, explorer, src_file, "file_2_1.txt")
    assert len(questions) == 1
    assert not src_file.exists()
    assert (sub_dir_2 / "file_2_1.txt").read_text() == "content of file 2_0"
    assert not warnings

    # ---- rename a file with the name of an existing file -> do not overwrite (No) -> suffix '_copy' ----
    questions.clear()
    question_answer = QMessageBox.StandardButton.No
    src_file = sub_dir_2 / "file_2_2.txt"
    rename_with_editor(qtbot, explorer, src_file, "file_2_1.txt")
    assert len(questions) == 1
    assert not src_file.exists()
    assert (sub_dir_2 / "file_2_1.txt").read_text() == "content of file 2_0"
    assert (sub_dir_2 / "file_2_1_copy.txt").read_text() == "content of file 2_2"
    assert not warnings

    # ---- only change the case of the name -> renamed without 'overwrite ?' question ----
    questions.clear()
    src_file = sub_dir_2 / "file_2_1_copy.txt"
    rename_with_editor(qtbot, explorer, src_file, "FILE_2_1_COPY.txt")
    assert not questions
    assert not warnings
    assert [p.name for p in sub_dir_2.iterdir() if p.name.lower() == "file_2_1_copy.txt"] == ["FILE_2_1_COPY.txt"]
    assert (sub_dir_2 / "FILE_2_1_COPY.txt").read_text() == "content of file 2_2"

    # ---- rename a directory with the name of an existing non-empty directory -> overwrite (Yes) ----
    # -> the content of both directories is merged (files with the same name are overwritten)
    questions.clear()
    question_answer = QMessageBox.StandardButton.Yes
    src_dir = sub_dir_2 / "sub_sub_folder_0"
    dst_dir = sub_dir_2 / "sub_sub_folder_1"
    (src_dir / "file_2_1_0.txt").write_text("new content")
    rename_with_editor(qtbot, explorer, src_dir, dst_dir.name)
    assert len(questions) == 1
    assert not warnings
    assert not src_dir.exists()
    assert sorted(p.name for p in dst_dir.iterdir()) == \
        ["file_2_0_0.txt", "file_2_0_1.txt", "file_2_0_2.txt",
         "file_2_1_0.txt", "file_2_1_1.txt", "file_2_1_2.txt"]
    assert (dst_dir / "file_2_1_0.txt").read_text() == "new content"
    assert (dst_dir / "file_2_1_1.txt").read_text() == "content of file 2_1_1"

    # ---- rename a file with the name of an existing directory -> error (before asking), nothing renamed ----
    questions.clear()
    src_file = sub_dir_2 / "file_2_1.txt"
    rename_with_editor(qtbot, explorer, src_file, "sub_sub_folder_2")
    assert not questions
    assert len(warnings) == 1
    assert "Cannot replace the directory" in warnings[0][2]
    assert src_file.read_text() == "content of file 2_0"
    assert sorted(p.name for p in (sub_dir_2 / "sub_sub_folder_2").iterdir()) == \
        ["file_2_2_0.txt", "file_2_2_1.txt", "file_2_2_2.txt"]
    warnings.clear()

    # ---- rename a directory with the name of an existing file -> error (before asking), nothing renamed ----
    src_dir = sub_dir_2 / "sub_sub_folder_2"
    rename_with_editor(qtbot, explorer, src_dir, "file_2_1.txt")
    assert not questions
    assert len(warnings) == 1
    assert "Cannot replace the file" in warnings[0][2]
    assert (src_dir / "file_2_2_0.txt").exists()
    assert (sub_dir_2 / "file_2_1.txt").read_text() == "content of file 2_0"
    warnings.clear()

