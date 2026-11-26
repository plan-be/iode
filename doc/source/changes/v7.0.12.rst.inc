Version 7.0.12
==============

In development

New Features 
------------


Improvements 
------------


Fixed Bugs
----------

- IODE REPORT: fixed a possible memory corruption in `$EqsSetCmt` and `$EqsSetInstrs`
  when the equation name is longer than 15 characters.

- IODE REPORT: fixed `$DataCalcVar`, `$DataUpdate`, `$DataAppend`, `$EqsSetCmt`,
  `$EqsSetInstrs`, `$StatUnitRoot` and `$ask` when nothing follows the first argument
  (the value was read after the end of the command line).

- IODE REPORT: a `%` in the question of `$ask` is now displayed as is (it was
  interpreted as a format code).

- IODE REPORT: fixed a crash of `$ModelExchange` without argument.

- IODE REPORT: fixed `$define NAME` without value: the macro is now empty (its value
  was read after the end of the command line).

- IODE REPORT: fixed a possible memory corruption when a report command name is longer
  than 30 characters (the name is now truncated to 30 characters).

- IODE REPORT: fixed possible memory corruptions with long formats in `{lec@format}`
  (40 characters or more), `@time()` (60 characters or more) and `@fmt()` (512 characters
  or more), and with long object names in `@ttitle()`, `@cvalue()`, `@vvalue()`,
  `@lvalue()`, `@ivalue()`, `@evalue()` and `@eqsample...()`.

- IODE REPORT: fixed `{$command}` without arguments and a `{` at the end of a report
  line (the line was read after its end).

- IODE REPORT: fixed `%*%` (all the report arguments): crash when the report has no
  arguments after a `$shift`, invalid result when `$shift` skips more arguments than
  the report has (now empty) or with a negative `$shift` (the report name was returned
  as an argument). Outside a report, `%*%` is now replaced by an empty string.

- IODE REPORT: fixed a crash of `@vvalue()` when the Variables workspace has no sample
  (it now returns an empty string).

- IODE REPORT: `@replace()` with an empty text to search now returns the text unchanged.


Miscellaneous 
-------------

