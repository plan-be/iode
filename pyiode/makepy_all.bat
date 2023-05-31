:: MAKEPY_ALL.BAT
:: --------------
:: Create iode.pyd for the predefined py39, py310 and py311 conda environments.
:: NOTES: make sure that these environments have numpy, pandas, larray and cython librairies installed.
:: 

set conda_envs="py39" "py310" "py311"

for %%i in (%conda_envs%) do (
    call makepy.bat %%i
)
