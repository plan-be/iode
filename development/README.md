To install the *in development* version of the *iode* or *iode-gui* Python package: 

1. open a browser and go to the page:

```bash
https://github.com/plan-be/iode/tree/development/development
```

2. Click on the *in development* package you want to install. 
The names are of type `iode-<version>-cp312-abi3-win_amd64.whl` for the *iode* 
package and of type `iode_gui-<version>-py3-none-any.whl` for the *iode-gui* package.
`<version>` represents the *in development* versions available in the development folder.

3. Once clicked on the `*.whl` file to download, click on the *View raw* link or on the *Download raw file* button to actually download the `*.whl` file. 


4. Open a console in which you have access to *pip* and go to the directory where the 
downloaded *.whl file is located. In the console, type:
```bash
pip install iode-<version>-cp312-abi3-win_amd64.whl
```
to install the *iode* package or:
```bash
pip install iode_gui-<version>-py3-none-any.whl
```
to install the *iode-gui* package.
