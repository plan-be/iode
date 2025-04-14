from iode.iode_cython import (cython_write_destination, cython_write_close, cython_write_flush, cython_write, 
                              cython_write_enum, cython_write_code_block, cython_write_paragraph, 
                              cython_write_title, cython_write_page_header, cython_write_page_footer)

from pathlib import Path
from typing import Union
from iode.iode_cython import WriteFileExt


def write_destination(filename: Union[str, Path], file_type: Union[WriteFileExt, str] = WriteFileExt.A2M):
    r'''
    Initialize a new output session

    Parameters
    ----------
    filename: str or Path
        Path to the file to write.
    file_type: WriteFileExt or str, optional
        Type of file to write. Possible values are: 'GDI', 'A2M', 'MIF', 'HTML', 'RTF', 'CSV', 'DUMMY'.
        Defaults to 'A2M'.

    Examples
    --------
    >>> from iode import WriteFileExt
    >>> from iode import (write_close, write_destination, write_flush, write, write_code_block, write_enum, 
    ...                   write_paragraph, write_title, write_page_footer, write_page_header)
    >>> from pathlib import Path
    >>> output_dir = getfixture('tmp_path')
    
    >>> def print_test_file(filename: str, file_type: WriteFileExt):
    ...     # open a new file
    ...     write_destination(filename, file_type)
    ...
    ...     write_page_header("IODE")
    ...     write_page_footer("- page %d - ")
    ...
    ...     # print a title
    ...     write_title("My Title (level 1)")
    ...
    ...     # print text
    ...     write("This is a simple text\n\n")
    ...
    ...     # print a new paragraph
    ...     write_paragraph("This is a paragraph with level 1\n")
    ...
    ...     # print a bulleted paragraph
    ...     write_enum("first item\n", 1)
    ...     write_enum("second item\n", 1)
    ...     write_enum("first sub-item\n", 2)
    ...     write_enum("second sub-item\n", 2)
    ...     write_enum("third item\n", 1)
    ...
    ...     # print code blocks
    ...     write_code_block("equations.estimate('2000Y1', '2010Y1', 'ACAF')\n")
    ...     write_code_block("equations.model_simulate('2000Y1', '2010Y1')\n")
    ...
    ...     write_title("Estimation/Simulation (level 2)", 2)
    ...
    ...     # print a new paragraph
    ...     write_paragraph("This is a paragraph with level 2\n", 2)
    ...
    ...     # # print a table (A2M format)
    ...     write(".tb\n")
    ...     write(".tborder 1\n")
    ...     write(".theader\n")
    ...     write("| Syntax | Description\n")
    ...     write(".tbody\n")
    ...     write("| estimate        | run an estimation of the coefficients of an equation\n")
    ...     write("| model_simulate  | run a complete simulation of the model\n")
    ...     write(".te\n")
    ...
    ...     # don't forget to close the file
    ...     write_close() 

    >>> filename = output_dir / "test.htm"
    >>> print_test_file(filename, WriteFileExt.HTML) 
    >>> with open(filename, "r") as f:          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    ...     print(f.read()) 
    <HTML>
    <HEAD>
    <TITLE>
    </TITLE>
    ...
    </HEAD>
    <BODY >
    <!STARTHEADER>
    <!ENDHEADER>
    <A NAME="REF1"></A><H2 class="tit_1"><FONT SIZE=+1><FONT FACE="Arial">My Title (level 1)</FONT></FONT></H2>
    <P ALIGN=JUSTIFY class="par_1"><FONT SIZE=-1><FONT FACE="Arial">This is a simple text</FONT></FONT></P> 
    <P ALIGN=JUSTIFY class="par_1"><FONT SIZE=-1><FONT FACE="Arial">This is a paragraph with level 1</FONT></FONT></P>
    <UL>
    <UL>
    <LI ALIGN=JUSTIFY class="enum_1"><FONT SIZE=-1><FONT FACE="Arial">first item</FONT></FONT></LI>
    <LI ALIGN=JUSTIFY class="enum_1"><FONT SIZE=-1><FONT FACE="Arial">second item</FONT></FONT></LI>        
    <UL>
    <LI ALIGN=JUSTIFY class="enum_2"><FONT SIZE=-1><FONT FACE="Arial">first sub-item</FONT></FONT></LI>     
    <LI ALIGN=JUSTIFY class="enum_2"><FONT SIZE=-1><FONT FACE="Arial">second sub-item</FONT></FONT></LI>    
    </UL>
    <LI ALIGN=JUSTIFY class="enum_1"><FONT SIZE=-1><FONT FACE="Arial">third item</FONT></FONT></LI>
    </UL>
    </UL>
    <PRE class="cmd_1"><FONT COLOR="#008000"><B>equations.estimate('2000Y1', '2010Y1', 'ACAF')<BR> <BR> </B></FONT></PRE>
    <PRE class="cmd_1"><FONT COLOR="#008000"><B>equations.model_simulate('2000Y1', '2010Y1')<BR> <BR> </B></FONT></PRE>
    <A NAME="REF2"></A><H3 class="tit_2"><FONT SIZE=+0><FONT FACE="Arial">Estimation/Simulation (level 2)<BR> </FONT></FONT></H3>
    <P ALIGN=JUSTIFY class="par_2"><FONT SIZE=-1><FONT FACE="Arial">This is a paragraph with level 2<BR> <BR> </FONT></FONT></P>
    <TABLE  class="A2mTable" BORDER=1 cellspacing="0" cellpadding="0">
    </TABLE>
    <!STARTFOOTER>
    <!ENDFOOTER>
    </BODY>
    </HTML>
    <BLANKLINE>

    >>> filename = output_dir / "test.a2m"
    >>> print_test_file(filename, WriteFileExt.A2M)
    >>> with open(filename, "r") as f:          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    ...     print(f.read()) 
    .pghead IODE
    .pgfoot - page %d -
    .par1 tit_1
    My Title (level 1)
    <BLANKLINE>
    This is a simple text
    <BLANKLINE>
    .par1 par_1
    This is a paragraph with level 1
    <BLANKLINE>
    <BLANKLINE>
    .par1 enum_1
    first item
    <BLANKLINE>
    <BLANKLINE>
    .par1 enum_1
    second item
    <BLANKLINE>
    <BLANKLINE>
    .par1 enum_2
    first sub-item
    <BLANKLINE>
    <BLANKLINE>
    .par1 enum_2
    second sub-item
    <BLANKLINE>
    <BLANKLINE>
    .par1 enum_1
    third item
    <BLANKLINE>
    <BLANKLINE>
    .lf_on
    .bl_on
    .par cmd_1
    equations.estimate('2000Y1', '2010Y1', 'ACAF')
    <BLANKLINE>
    <BLANKLINE>
    .lf_on
    .bl_on
    .par cmd_1
    equations.model_simulate('2000Y1', '2010Y1')
    <BLANKLINE>
    <BLANKLINE>
    .par1 tit_2
    Estimation/Simulation (level 2)
    <BLANKLINE>
    .par1 par_2
    This is a paragraph with level 2
    <BLANKLINE>
    <BLANKLINE>
    .tb
    .tborder 1
    .theader
    | Syntax | Description
    .tbody
    | estimate        | run an estimation of the coefficients of an equation
    | model_simulate  | run a complete simulation of the model
    .te
    <BLANKLINE>

    >>> filename = output_dir / "test.csv"
    >>> print_test_file(filename, WriteFileExt.CSV)
    >>> with open(filename, "r") as f:          # doctest: +ELLIPSIS, +NORMALIZE_WHITESPACE
    ...     print(f.read()) 
    "My Title (level 1)"
    "This is a simple text"
    "This is a paragraph with level 1"
    " - first item"
    " - second item"
    " - first sub-item"
    " - second sub-item"
    " - third item"
    "equations.estimate('2000Y1', '2010Y1', 'ACAF')
    <BLANKLINE>
     "
    "equations.model_simulate('2000Y1', '2010Y1')
    <BLANKLINE>
     "
    "Estimation/Simulation (level 2)
     "
    "This is a paragraph with level 2
    <BLANKLINE>
     "
    <BLANKLINE>
    '''
    if not isinstance(filename, (str, Path)):
        raise TypeError("'filename': Expected value of type str or Path. "
                        f"Got value of type {type(filename).__name__} instead")
    if isinstance(filename, Path):
        filename = filename.resolve()
        filename = str(filename)

    if not isinstance(file_type, (WriteFileExt, str, int)):
        raise TypeError("'file_type': Expected value of type WriteFileExt, str or int. "
                        f"Got value of type {type(file_type).__name__} instead")
    if isinstance(file_type, str):
        file_type = file_type.upper()
        file_type = WriteFileExt[file_type]
    file_type = int(file_type)

    cython_write_destination(filename, file_type)

def write_flush():
    """
    Flush the output session buffer

    See Also
    --------
    write_destination
    """
    success = cython_write_flush()
    if not success:
        raise RuntimeError("Nothing to flush")

def write_close():
    """
    End an output session

    See Also
    --------
    write_destination
    """
    cython_write_close()

def write(text: str=""):
    """
    Send a string into the current opened buffer

    Parameters
    ----------
    text: str, optional
        Text to print. If empty, a new line is printed.

    See Also
    --------
    write_destination
    """
    if not isinstance(text, str):
        raise TypeError("'text': Expected value of type str. "
                        f"Got value of type {type(text).__name__} instead")
    success = cython_write(text)
    if not success:
        raise RuntimeError(f"Could not write '{text}' to the file")
    
def write_enum(text: str, level: int = 1):
    """
    Print a bulleted paragraph of the given level

    Parameters
    ----------
    text: str
        Text to print after the bullet.
    level: int, optional
        Level of the bullet series. 1 is the highest level.
        Defaults to 1.

    See Also
    --------
    write_destination
    """ 
    if not isinstance(text, str):
        raise TypeError("'text': Expected value of type str. "
                        f"Got value of type {type(text).__name__} instead")
    if not text:
        raise ValueError("'text': Expected a non-empty string")
    if not isinstance(level, int):
        raise TypeError("'level': Expected value of type int. "
                        f"Got value of type {type(level).__name__} instead")
    cython_write_enum(level)
    if len(text) > 0:
        write(text + "\n\n")
    
def write_code_block(text: str, level: int = 1):
    """
    Start a code block of the given level or end the block if level < 0

    Parameters
    ----------
    text: str
        Text to print in the code block.
    level: int, optional
        Level of the code block. 1 is the highest level.
        Defaults to 1.

    See Also
    --------
    write_destination
    """ 
    if not isinstance(text, str):
        raise TypeError("'text': Expected value of type str. "
                        f"Got value of type {type(text).__name__} instead")
    if not text:
        raise ValueError("'text': Expected a non-empty string")
    if not isinstance(level, int):
        raise TypeError("'level': Expected value of type int. "
                        f"Got value of type {type(level).__name__} instead")
    cython_write_code_block(level)
    if len(text) > 0:
        write(text + "\n\n")
    
def write_paragraph(text: str, level: int = 1):
    """
    Print a paragraph of the given level

    Parameters
    ----------
    text: str
        Text to print in the paragraph.
    level: int, optional
        Level of the paragraph. 1 is the highest level.
        Defaults to 1.

    See Also
    --------
    write_destination
    """ 
    if not isinstance(text, str):
        raise TypeError("'text': Expected value of type str. "
                        f"Got value of type {type(text).__name__} instead")
    if not text:
        raise ValueError("'text': Expected a non-empty string")
    if not isinstance(level, int):
        raise TypeError("'level': Expected value of type int. "
                        f"Got value of type {type(level).__name__} instead")
    cython_write_paragraph(level)
    if len(text) > 0:
        write(text + "\n\n")
    
def write_title(title: str, level: int = 1):
    """
    Print a title of the given level

    Parameters
    ----------
    title: str
        Title to print.
    level: int, optional
        Level of the title. 1 is the highest level.
        Defaults to 1.

    See Also
    --------
    write_destination
    """ 
    if not isinstance(title, str):
        raise TypeError("'title': Expected value of type str. "
                        f"Got value of type {type(title).__name__} instead")
    if not title:
        raise ValueError("'title': Expected a non-empty string")
    if not isinstance(level, int):
        raise TypeError("'level': Expected value of type int. "
                        f"Got value of type {type(level).__name__} instead")
    cython_write_title(level)
    if len(title) > 0:
        write(title + "\n\n")

def write_page_header(text: str):
    """
    Define the page header as from the current page

    Parameters
    ----------
    text: str
        Text to print in the header.

    See Also
    --------
    write_destination
    """ 
    if not isinstance(text, str):
        raise TypeError("'text': Expected value of type str. "
                        f"Got value of type {type(text).__name__} instead")
    cython_write_page_header(text)
    
def write_page_footer(text: str):
    """
    Define the page footer as from the current page

    Parameters
    ----------
    text: str
        Text to print in the footer.
    
    See Also
    --------
    write_destination
    """ 
    if not isinstance(text, str):
        raise TypeError("'arg': Expected value of type str. "
                        f"Got value of type {type(text).__name__} instead")
    cython_write_page_footer(text)
