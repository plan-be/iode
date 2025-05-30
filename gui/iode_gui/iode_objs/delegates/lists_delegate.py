from .base_delegate import BaseDelegate

from iode import IodeType


class ListsDelegate(BaseDelegate):
    """
    ListsDelegate class that inherits from BaseDelegate.
    """

    def __init__(self, parent=None):
        """
        Constructor for ListsDelegate.

        :param parent: The parent object.
        """
        super().__init__(IodeType.LISTS, parent)
