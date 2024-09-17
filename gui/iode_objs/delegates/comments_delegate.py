from base_delegate import BaseDelegate

from iode import IodeTypes

class CommentsDelegate(BaseDelegate):
    """
    A delegate class for handling comments.

    This class inherits from the BaseDelegate class and provides functionality
    for managing comments.
    """

    def __init__(self, parent=None):
        """
        Initializes a new instance of the CommentsDelegate class.

        :param parent: The parent object of this CommentsDelegate instance.
        """
        super().__init__(IodeTypes.COMMENTS, parent)
