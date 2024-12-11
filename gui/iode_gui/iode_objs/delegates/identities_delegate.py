from .base_delegate import BaseDelegate

from iode import IodeType


class IdentitiesDelegate(BaseDelegate):
    """
    A delegate class for handling identities.
    Inherits from the BaseDelegate class.
    """

    def __init__(self, parent=None):
        """
        Initializes the IdentitiesDelegate object.
        Calls the constructor of the BaseDelegate class with the IDENTITIES constant.
        """
        super().__init__(IodeType.IDENTITIES, parent)
