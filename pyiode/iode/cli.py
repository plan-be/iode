import sys
import argparse
from iode import execute_report, __version__
from .iode_cython import (initialize_cli, finish_cli, set_log_file_cli, always_continue, 
                          get_total_allocated_memory, alloc_doc_loop)


description = f"IODE CLI v. {__version__}"


def main():
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('-nbrun', type=int, help='how many times the report must be executed')
    parser.add_argument('-alloclog', type=str, help='log allocations not freed at the end of the report execution')
    parser.add_argument('-v', '--verbose', action='store_true', help='give more detailed messages')
    parser.add_argument('-y', '--forceyes', action='store_true', help='when a question is asked, automatically answer yes')
    parser.add_argument('reportfile', type=str, help='report filename')
    parser.add_argument('args', nargs=argparse.REMAINDER, help='optional report arguments')
    args = parser.parse_args()

    initialize_cli()

    verbose = args.verbose
    nb_run = args.nbrun if args.nbrun else 0
    if args.alloclog:
        set_log_file_cli(args.alloclog)
    if args.forceyes:
        always_continue()
    if args.reportfile:
        report_name = args.reportfile
        report_args = args.args

    if nb_run < 1 or report_name is None:
        print(description)
        print("nothing to do ...")
        sys.exit(1)

    if verbose:
        print(description)
        print(f"Report name : {report_name}")
        print(f"Report args : {report_args}")
        print(f"Number of runs : {nb_run}")
        print(f"forces yes : {args.forceyes}")
        if args.alloclog:
            print(f"log file : {args.alloclog}")
        print()

    for i in range(nb_run):
        rc = execute_report(report_name, report_args)
        if verbose:
            memory_used = get_total_allocated_memory()
            print(f"\n*****Run {i + 1}/{nb_run} *****\nrc = {rc} "
                  f"-- total residual allocation : {memory_used} bytes allocated")

    finish_cli()

    if verbose:
        memory_used = get_total_allocated_memory()
        print(f"After last run : {memory_used} bytes allocated")

    alloc_doc_loop()

    sys.exit(rc)


if __name__ == "__main__":
    main()
