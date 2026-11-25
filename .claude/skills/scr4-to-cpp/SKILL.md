---
name: scr4-to-cpp
description: Replace calls to legacy scr4 C functions (SCR_*, U_*, SW_*) in the IODE C++ API (api/) by modern C++ code using std::string, std::vector, smart pointers, the std library or the helpers of api/utils/utils.h. Use when refactoring a function of api/ that still uses char*/char** tables, SCR_malloc/SCR_free, SW_nalloc/SW_nfree, SCR_vtom/SCR_mtov, SCR_stracpy, SCR_strip, SCR_sqz, U_pos, U_is_in, etc.
---

# Replacing scr4 functions by C++ code

The `scr4/` directory is a legacy K&R C library. New code in `api/` must use the STL and the
helpers of [api/utils/utils.h](../../../api/utils/utils.h) instead. This skill lists, for each
scr4 function, its exact behaviour and its C++ replacement.

**Do not change `scr4/` itself.** Only the callers in `api/` are migrated.

## General rules

1. **Read the scr4 source before replacing a call.** scr4 functions often have subtle
   behaviours (empty tokens skipped, only `' '` removed, accented CP850 letters converted,
   in-place modification, `NULL` accepted...). The tables below summarize them, but the
   source in `scr4/strs/` and `scr4/base/` is the reference.
2. **Preserve the behaviour**, unless the user asks otherwise. When a behaviour changes
   (e.g. `NULL` -> empty string), say it explicitly in the final report and the commit message.
3. **Ownership changes must be complete.** A `char*` allocated with `SCR_malloc`/`SW_nalloc`
   is freed with `SCR_free`/`SW_nfree` (they are interchangeable: `SW_nalloc` calls
   `SCR_malloc`, `SW_nfree` calls `SCR_free`). Memory allocated with `new[]` (e.g. by
   `copy_char_array()` or `vector_to_double_char()` from utils.h) must **never** be freed with
   `SCR_free`/`SCR_free_tbl`, and vice versa.
4. **Structs containing C++ members** (`std::string`, `std::vector`, `std::shared_ptr`) must be
   created with `new`/`std::make_shared`, never with `SCR_malloc`/`RP_alloc`
   (no constructor would run).
5. **`goto` over initialisations**: legacy functions often use `goto done`. A C++ object with
   a non-trivial constructor (`std::string`, `std::vector`...) declared between the `goto` and
   its label is a compile error (MSVC C2362). Declare it before the first `goto`, put it in its
   own `{ }` block, or replace the `goto done` paths by direct `return`s.
6. **Functions still taking `char*`**: when a callee still expects a modifiable `char*`, pass
   `str.data()` (C++17). If the callee shrinks the string by writing a `'\0'`, `str.size()` is
   no longer accurate: only use the string as a C string (`c_str()`) afterwards.
   When it expects an allocated `char*` it will free, pass `SCR_stracpy(str.c_str())`.
7. **Text encoding**: strings in reports and IODE objects are CP850 (OEM) encoded. scr4
   functions such as `SCR_upper`, `SCR_is_alpha` or `SCR_is_anum` know the CP850 accented
   letters; `std::toupper`/`std::isalpha` (and utils.h `to_upper()`) do **not**. See the
   *Case and character classes* table.

## Memory and string copies

| scr4 | C++ replacement | Notes |
|------|-----------------|-------|
| `SCR_malloc(n)`, `SW_nalloc(n)` | `std::string`, `std::vector<T>`, `std::make_shared<T>()`, `new T()` | Both zero-fill the memory. Return `NULL` if `n == 0`. |
| `SCR_free(p)`, `SW_nfree(p)` | destructor / `delete` / nothing | Both accept `NULL`. |
| `SCR_realloc(p, elsize, oldn, newn)`, `SW_nrealloc(p, old, new)` | `vec.resize(newn)`, `str.append(...)`, `str.push_back(...)` | New elements are zero-filled. |
| `SCR_stracpy(in)` | `std::string out(in);` | Returns `NULL` if `in == NULL`: guard with `in ? std::string(in) : std::string()`. |
| `SCR_strfacpy(&old, in)` | `old = in;` | Frees `old` then copies `in`. |
| `SCR_stracat(in, add)` | `std::string res = in + add;` | Returns `NULL` if both are empty or `NULL`. |
| `SCR_strafcat(in, add)` | `in += add;` | Frees `in`. Returns `in` unchanged if `add == NULL`. |
| `SCR_strlcpy(out, in, n)` | `out = std::string(in).substr(0, n);` | Copies at most `n` chars, always 0-terminated. `in == NULL` gives `""`. |

## Tables of strings (`char**` terminated by a `NULL` pointer)

| scr4 | C++ replacement | Notes |
|------|-----------------|-------|
| `char** tbl` | `std::vector<std::string>` | |
| `SCR_tbl_size(tbl)` | `vec.size()` | 0 if `tbl == NULL`. |
| `SCR_free_tbl(tbl)` | nothing (destructor) | Accepts `NULL`. |
| `SCR_add_ptr(&tbl, &n, txt)` | `vec.push_back(txt);` | `SCR_add_ptr(&tbl, &n, NULL)` only adds the final `NULL` terminator: drop it. The global `SCR_ADD_PTR_CHUNCK` only tunes reallocations: drop it too. |
| `SCR_copy_tbl(tbl)` | `std::vector<std::string> copy = vec;` | |
| `SCR_vtom(str, sep)` | `split_skip_empty(str, std::string(1, sep))` (utils.h) | Empty tokens are skipped: `"A,,B,"` -> `{"A", "B"}`. `NULL` -> `NULL`; `""` -> empty table. |
| `SCR_vtoms(str, seps)` | `split_skip_empty(str, seps)` (utils.h) | Same as `SCR_vtom` with several separators. |
| `SCR_vtoms2(str, seps, 0)` | `split_multi(str, seps)` (utils.h) | Keeps empty tokens. With `supdbl = 1`, same as `SCR_vtoms`. |
| `SCR_vtoms3(str, seps, supdbl)` | `split_multi_quoted(str, seps, '"')` (utils.h) | Text between `"` is not split. Check the edge cases: `SCR_vtoms3` has a known bug with `"abc","def"`. |
| `SCR_mtov(tbl, sep)` | `join(vec, std::string(1, sep))` (utils.h) | `NULL` table -> `NULL`. |
| `SCR_mtovs(tbl, seps)` | `join(vec, seps)` (utils.h) | |
| `ScrStrIsIn(tbl, f)` | `std::find(vec.begin(), vec.end(), f) != vec.end()` | |
| `SCR_text(str, seps, lg)` | `text_to_lines(str, seps, lg)` (utils.h) | Wraps a text into lines of at most `lg` chars, cut after the last char of `seps`. See the details below. |

Do **not** use `split(str, sep)` or `split_multi(str, seps)` (utils.h) to replace `SCR_vtom` /
`SCR_vtoms`: they **keep** the empty tokens and return `{""}` for an empty string.
When the separators are exactly the white spaces (`" \t\n\r\v\f"`), `split_whitespace(str)`
(utils.h) gives the same result as `split_skip_empty`.

### `SCR_text(str, seps, lg)`: wrap a text into lines

Behaviour of `SCR_text()` (`scr4/strs/s_sttext.c`), for each line:

1. skip the leading white spaces (`SCR_is_space`: `' '`, `\t`, `\n`, `\r`, `\v`, `\f`);
2. read at most `lg` chars, stopping before a `'\n'`, and remember the position of the last
   char of `seps` found at an index `> 0`;
3. if the line is too long (more chars follow) and a separator was found, the line ends
   **after** that separator (the separator is kept, even if it is a space); otherwise it ends
   before the `'\n'`, at the end of the text, or after exactly `lg` chars (a word longer than
   `lg` is cut).

Notes:
- the trailing spaces of a line are **not** removed: with `seps = ", ;"` and `lg = 15`,
  `"Jean, Marc, Pierre, Louis"` gives `"Jean, Marc, "` (with a trailing space; the example in
  the scr4 comment is wrong) then `"Pierre, Louis"`;
- a text containing only white spaces gives an empty table;
- `SCR_text(NULL, ...)` crashes and `SCR_text(str, seps, lg <= 0)` **loops forever**.

`text_to_lines(str, seps, lg)` (utils.h) has exactly the same behaviour (tested in
`UtilsTest.textToLines`), except that it returns an empty vector if `lg <= 0`. It uses the
fixed set of white spaces of `SCR_is_space` (not `std::isspace`, which depends on the locale).

Typical use: `K_wrap()` (`api/utils/yy.cpp`) wraps with `SCR_text()` then joins the lines with
`'\n'`: `join(text_to_lines(in, ";,*+-/) ", lg), "\n")`.

To pass a `std::vector<std::string>` to a function still expecting a `char**`:
`vector_to_double_char(vec)` (utils.h) allocates with `new[]`, so free each element with
`delete[]` and then the table with `delete[]`. **Never** free it with `SCR_free_tbl`.

## Search and comparison

| scr4 | C++ replacement | Notes |
|------|-----------------|-------|
| `U_pos(ch, str)` | `str.find(ch)` (`std::string::npos` instead of `-1`) | Crashes if `str == NULL`. `U_pos('\0', str)` is `-1`. |
| `U_is_in(ch, str)` | `string_contains(str, ch)` (utils.h) or `std::string_view(str).find(ch) != npos` | `U_is_in('\0', str)` is `0`. With a `std::string`, `find('\0')` can match an embedded `'\0'`. |
| `U_index(str, sstr)` | `str.find(sstr)` | Returns `-1` if not found. |
| `get_pos_in_char_array(str, ch)` (utils.h) | `str.find(ch)` | Already a C-style helper. |
| `SCR_cstrcmp(s1, s2)` | compare `SCR_upper`-equivalent copies (see *Case*) | Case-insensitive, **including accented CP850 letters** (é == E). `NULL` handling: both `NULL` -> 0, one `NULL` -> -1. `to_upper(a) == to_upper(b)` is only correct for ASCII strings. |
| `SCR_cstrncmp(s1, s2, lg)` | same as above with `.compare(0, lg, ...)` | |

## Trimming, squeezing, justification (all in place in scr4)

| scr4 | C++ replacement | Notes |
|------|-----------------|-------|
| `SCR_strip(str)` | `str.erase(str.find_last_not_of(...) + 1)`: remove the trailing chars `<= ' '` | Removes **all** trailing chars `<= ' '` (spaces, tabs, `\n`, control chars), only at the **end**. Not the same as `trim()` (both ends, whitespace only). |
| `SCR_pstrip(str, punct)` | `str.erase(str.find_last_not_of(punct) + 1)` | Removes the trailing chars in `punct`. Guard `npos` (all chars in `punct` -> `""`). |
| `U_ljust_text(str)` | `str = str.substr(n) + std::string(n, ' ')` where `n` = number of leading `SCR_is_space` chars | Keeps the **length** (spaces moved to the end). Does nothing if the string is only spaces. Usually followed by `SCR_strip`: then `trim()` (utils.h) is equivalent. |
| `U_rjust_text(str)` | symmetric of `U_ljust_text` | Keeps the length. |
| `U_shift_text(str, n)` | `n > 0`: `str.substr(n) + std::string(n, ' ')`; `n < 0`: `std::string(-n, ' ') + str.substr(0, size + n)` | Keeps the length. |
| `U_ljust_text(str); SCR_strip(str);` | `trim(str)` (utils.h) | `trim` uses `" \t\n\r\f\v"`; `SCR_strip` also removes trailing control chars. |
| `SCR_sqz(str)` | `str.erase(std::remove(str.begin(), str.end(), ' '), str.end());` | Removes only `' '` (not tabs). |
| `U_sqz_text(str)` | `SCR_sqz` equivalent + pad with `' '` to the original length | |
| `SCR_asqz(str, chars)` | `std::erase_if(str, [&](unsigned char c){ return !(SCR_is_anum(c) \|\| chars.find(c) != std::string::npos); });` | Keeps letters (**including CP850 accented letters**), digits and the chars of `chars`. Keep calling `SCR_is_anum` until a CP850-aware C++ helper exists. |

## Replacement

| scr4 | C++ replacement | Notes |
|------|-----------------|-------|
| `SCR_replace(txt, repl, by)` | `replace_all(str, repl, by)` (utils.h) | In place. Replaces all occurrences left to right, without overlap and without rescanning the inserted text: `"AAAA","AA","A"` -> `"AA"`. Returns the number of replacements. scr4 requires `txt` to be large enough; `std::string` grows by itself. |
| `SCR_areplace(txt, repl, by)` | `std::string res = txt; replace_all(res, repl, by);` | Result allocated, `txt` unchanged. |
| `SCR_replace_word(txt, repl, by)`, `SCR_areplace_word` | same, but only when `repl` is not preceded or followed by a letter (CP850 included), a digit or `'_'` | `"AB+ABC+AB_C+AB/2"`, `"AB"` -> `"XXXX"` gives `"XXXX+ABC+AB_C+XXXX/2"`. |

## Case and character classes (CP850 aware)

| scr4 | C++ replacement | Notes |
|------|-----------------|-------|
| `SCR_lower(str)`, `SCR_lower_char(ch)` | `to_lower(str)` (utils.h) **only in the "C" locale** | Only `A-Z` are converted. `std::tolower` (used by `to_lower`) depends on the current C locale, and the Python interpreter sets `LC_CTYPE` to the user locale at startup: in the `iode` Python package, it may also convert accented letters. To keep the exact behaviour, keep calling `SCR_lower` on `str.data()`. |
| `SCR_upper(str)`, `SCR_upper_char(ch)` | `to_upper(str)` (utils.h) **only for ASCII text** | `SCR_upper_char` also converts the CP850 accented letters to the **unaccented** capital (`é`->`E`, `ç`->`C`, `Ä`->`A`...). For report text or object comments, keep `SCR_upper_char` (called per char on `str.data()`) or write a CP850-aware helper. |
| `SCR_is_upper(ch)` | `ch >= 'A' && ch <= 'Z'` | ASCII only. |
| `SCR_is_lower(ch)` | `SCR_upper_char(ch) != ch` | `a-z` **and** CP850 accented letters. |
| `SCR_is_alpha(ch)` | no std equivalent (`std::isalpha` ignores CP850) | `A-Z`, `a-z` and CP850 accented letters. |
| `SCR_is_num(ch)` | `ch >= '0' && ch <= '9'` (or `std::isdigit((unsigned char) ch)`) | |
| `SCR_is_anum(ch)` | `SCR_is_num(ch) \|\| SCR_is_alpha(ch)` | Does **not** accept `'_'`. |
| `SCR_is_space(ch)` | `std::string_view(" \t\n\r\v\f").find(ch) != std::string_view::npos` | `' '`, `\t`, `\n`, `\r`, `\v`, `\f`. Same set as `std::isspace` in the "C" locale only: in another locale (e.g. inside Python), `std::isspace` may also accept other chars (`0xA0`...). |

Always cast to `unsigned char` before calling `std::isxxx`/`std::toupper`: CP850 chars are
negative as `char`, which is undefined behaviour. The `std::isxxx`/`std::toxxx` functions
depend on the current C locale (see `SCR_lower` above): prefer explicit character sets or the
scr4 functions when the exact behaviour matters.

## File names and files

| scr4 | C++ replacement | Notes |
|------|-----------------|-------|
| `SCR_cat_ext(res, filename, ext)` | `add_file_extension(filename, "." + ext)` (utils.h) | Adds the extension only if there is none. `scr4` takes `ext` **without** the dot. |
| `SCR_change_ext(res, filename, ext)` | `std::filesystem::path(filename).replace_extension(ext).string()` | Always replaces the extension. `ext == ""` removes it. scr4 also stops at `':'` and `'!'`. Or use `set_file_extension()` (api/) when available. |
| `SCR_split_dir(dir, file)` | `std::filesystem::path p(...); p.parent_path(); p.filename();` | |
| `SCR_fullpath(in, out)` | `std::filesystem::absolute(path)` | |
| `SCR_getcwd(...)` | `std::filesystem::current_path()` | |
| `SCR_rename(a, b)` | `std::filesystem::rename(a, b)` | Throws on error: wrap in `try/catch` or use the `std::error_code` overload. |
| `SCR_copy_file(a, b)` | `std::filesystem::copy_file(a, b, std::filesystem::copy_options::overwrite_existing)` | Check the scr4 source for the overwrite behaviour. |
| reading a text file line by line with `SCR_add_ptr` | `std::ifstream` + `std::getline`, or `fopen`/`getc` + `std::string` | Keep the existing maximum line length checks (e.g. `RP_LINELENGTH`). |

## Dates

| scr4 | C++ replacement | Notes |
|------|-----------------|-------|
| `SCR_current_date()`, `_SCR_current_date()` | `current_date_to_long()` (utils.h) | Current **local** date as a `long` `YYYYMMDD` (e.g. `20261125`), 0 if the local date cannot be computed (same as scr4). Uses `std::time` + `localtime_s`/`localtime_r` (thread-safe). Do not use `std::chrono::floor<days>(system_clock::now())`: it gives the **UTC** date (different from the local date around midnight). |
| `SCR_long_to_fdate(date, str, fmt)` | `long_to_date(date, fmt)` (utils.h) | `date` is a `long` `YYYYMMDD`. Returns a `std::string` instead of filling `str`. `fmt` defaults to `"DD/MM/YYYY"` (scr4: `fmt == NULL`). Same results as `SCR_long_to_fdate` for strictly positive dates (tested in `UtilsTest.longToDate`). **Throws `std::invalid_argument` if `date <= 0`** (scr4 formats them as `"00/00/0000"` or with `'-'` signs): check how the caller handles an empty or invalid date before replacing a call. |

Format rules of `SCR_long_to_fdate` / `long_to_date`:
- only `Y`, `M` and `D` (upper or lower case) are interpreted, **anywhere in `fmt`**: every
  such letter is a date letter, even inside a word (`"Date: dd"` gives `"3ate: 1d"`);
- `n` Y's give the last `n` digits of the year (formatted on 4 digits); Y's beyond the 4th are
  kept as is: `"yy"` -> `"90"`, `"YYYYY"` -> `"1990Y"`;
- the first 2 M's (D's) are replaced by the 2 digits of the month (day); the next ones are
  kept as is;
- the example in the scr4 comment is wrong: `SCR_long_to_fdate(19901231L, buf, "mm-dd-yy")`
  gives `"12-31-90"`, not `"12/31/90"`.

`SCR_long_to_fdate(SCR_current_date(), buf, fmt)` becomes `long_to_date(current_date_to_long(), fmt)`.
Note that if `current_date_to_long()` fails, it returns 0 and `long_to_date()` then **throws** (scr4
printed `"00/00/0000"`).

## No direct equivalent (keep, or ask the user)

These functions have no simple drop-in replacement. Keep them unless the user asks to migrate
them, and then read their source first:

- number formatting: `SCR_fmt_dbl`, `SCR_fmt_long` (IODE-specific formats).
- dates and times: `SCR_current_time`, `SCR_long_to_ftime`, `SCR_fdate_to_long`
  (for `SCR_current_date` and `SCR_long_to_fdate`, see *Dates* above).
- code pages: `SCR_OemToAnsi`, `SCR_AnsiToOem`, `SCR_ConvertToUTF8`: see
  `oem_to_utf8()` / `utf8_to_oem()` in utils.h for OEM <-> UTF-8.
- a2m output (`SCR_fprintf_esc`...), swap allocator internals (`SW_init`, `SW_end`,
  `SW_status`, `SW_getptr`), `SCR_grep`, `SCR_HttpGetFile`, `SCR_sleep`, `SCR_panic`.

## Workflow

1. List the callers of the function to migrate with `git grep -n "<name>" -- api pyiode tests gui`.
   Ignore `archives/` and the build directories.
2. Read the function, its callers and the scr4 functions it uses.
3. Change the signature and the callers together. When a `char*` result is replaced by a
   `std::string`, check how callers detected errors (`NULL` result) and keep an equivalent
   signal (empty string, return code, exception).
4. **Preserve the file encoding and line endings.** Source files in `api/` use CRLF and may
   contain CP850/Latin-1 accented chars in comments. Use the Edit tool. Avoid `sed -i`: on
   Windows it converts CRLF to LF. If you must script a replacement, read and write the file
   with the ISO-8859-1 encoding (byte-preserving) and check with
   `git ls-files --eol <file>` (expected: `w/crlf`).
5. Build and test:
   ```
   cmake --build --preset windows-debug --target iode_c_api
   cmake --build --preset windows-debug --target test_debug_c_api
   ctest --preset c-api-windows-debug
   ```
   For a change in the report engine, `ctest --preset c-api-windows-debug -R Report` is a
   quick first check. Run the full suite before proposing a commit.
6. Commit message: prefix `C++ API:`, then `(FunctionName)` and an indented bullet list
   (see CLAUDE.md). Mention every behaviour change.
