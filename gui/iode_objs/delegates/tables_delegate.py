from base_delegate import BaseDelegate

from iode import IodeTypes


class TablesDelegate(BaseDelegate):
    """
    A subclass of BaseDelegate specifically for handling tables.
    """

    def __init__(self, parent=None):
        """
        Initializes a new instance of the TablesDelegate class.

        :param parent: The parent object of this delegate.
        """
        super().__init__(IodeTypes.TABLES, parent)
