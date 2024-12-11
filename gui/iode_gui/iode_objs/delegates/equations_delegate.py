from .base_delegate import BaseDelegate

from iode import IodeType


class EquationsDelegate(BaseDelegate):
    """
    A delegate class for handling equations.
    Inherits from BaseDelegate.
    """

    def __init__(self, parent=None):
        """
        Initializes the EquationsDelegate object.

        :param parent: The parent object.
        """
        super().__init__(IodeType.EQUATIONS, parent)
