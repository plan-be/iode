from pathlib import Path
from pypdf import PdfReader
from typing import Union, Tuple, List


def extract_text_from_pdf(pdf_path, pages: Union[int, Tuple[int, int], List[int]] = None) -> str:
    """
    Extracts text from a PDF file.

    Args:
        pdf_path (str): The path to the PDF file.
        pages (Union[int, Tuple[int, int], List[int]], optional): 
            The page numbers to extract text from. 
            If an integer is provided, it extracts from that single page.
            If a tuple is provided, it extracts from the range of pages.
            If a list is provided, it extracts from the specified pages.
            Defaults to None, which means all pages will be extracted.

    Returns:
        str: The extracted text from the PDF.
    """
    try:
        reader = PdfReader(pdf_path)
        text = ""
        if pages is None:
            # Extract text from all pages
            pages = range(len(reader.pages))
        elif isinstance(pages, int):
            # Single page extraction
            pages = [pages-1]
        elif isinstance(pages, tuple):
            # Range of pages extraction
            pages = range(pages[0]-1, pages[1]+1)
        for page_number in pages:
            if page_number < 0 or page_number >= len(reader.pages):
                raise IndexError(f"Page number {page_number} is out of range.")
            print(f"Extracting text from page {page_number}...")
            page_number -= 1  # Adjust for zero-based indexing
            text += reader.pages[page_number].extract_text()
        return text
    except Exception as e:
        print(f"An error occurred while extracting text from the PDF: {e}")
        return ""

def export_text_to_file(text: str, output_path: str) -> None:
    """
    Exports the extracted text to a file.

    Args:
        text (str): The text to export.
        output_path (str): The path to the output file.
    """
    try:
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(text)
        print(f"Text successfully exported to {output_path}")
    except Exception as e:
        print(f"An error occurred while exporting text to file: {e}")


current_dir = Path(__file__).parent
pdf_path = current_dir / "build" / "iode.pdf"
output_path = current_dir / "extracted_text.txt"
if not pdf_path.exists():
    raise FileNotFoundError(f"The PDF file {pdf_path} does not exist.")
pdf_path = str(pdf_path.absolute())
output_path = str(output_path.absolute())

# execution commands in IODE reports
pages = (159, 177)

pdf_text = extract_text_from_pdf(pdf_path, pages)
export_text_to_file(pdf_text, output_path)
