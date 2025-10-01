import sys
import numpy as np
import mooarchives

def usage():
    print("""
Usage: python archiver.py [OPTIONS]

Options:
  -t integer         : archive type (0=HV, 1=Dominating, 2=Unbounded)
  -N integer         : archive max size
  -f filename        : input file with solutions
  -len integer       : sequence length (max number of solutions to read)
  -s integer         : random seed
  -o filename_prefix : output file prefix (for saving archive after each add)
  -h                 : print help
""")

def parse_args(argv):
    args = {
        "archive_type": 0,
        "max_size": 100,
        "dimension": None,
        "seq_length": None,
        "seq_filename": None,
        "seed": None,
        "fileprefix": None,
    }
    i = 1
    while i < len(argv):
        if argv[i] == "-h":
            usage()
            sys.exit(0)
        elif argv[i] == "-t":
            args["archive_type"] = int(argv[i+1])
            i += 2
        elif argv[i] == "-N":
            args["max_size"] = int(argv[i+1])
            i += 2
        elif argv[i] == "-len":
            args["seq_length"] = int(argv[i+1])
            i += 2
        elif argv[i] == "-f":
            args["seq_filename"] = argv[i+1]
            i += 2
        elif argv[i] == "-s":
            args["seed"] = int(argv[i+1])
            i += 2
        elif argv[i] == "-o":
            args["fileprefix"] = argv[i+1]
            i += 2
        else:
            print(f"Unknown option: {argv[i]}")
            usage()
            sys.exit(1)
    return args

def read_solution(line):
    try:
        values = [float(x) for x in line.strip().split()]
        if len(values) == 0:
            return None
        return values
    except Exception:
        return None

def main():
    args = parse_args(sys.argv)
    if not args["seq_filename"]:
        print("error: no input file (use -f file)")
        sys.exit(1)

    # Read all solutions from file
    with open(args["seq_filename"], "r") as f:
        lines = f.readlines()

    solutions = []
    for line in lines:
        sol = read_solution(line)
        if sol is not None:
            solutions.append(sol)
    if not solutions:
        print(f"error reading input file {args['seq_filename']}")
        sys.exit(1)

    dimension = len(solutions[0])
    if args["dimension"] is not None:
        dimension = args["dimension"]

    # Select archive type
    rng = mooarchives.Random(args["seed"] if args["seed"] is not None else 42)
    archive_type = args["archive_type"]
    if archive_type == 2:
        archive = mooarchives.UnboundedArchive(dimension)
    elif archive_type == 1:
        archive = mooarchives.DominatingArchive(args["max_size"], dimension)
    elif archive_type == 0:
        archive = mooarchives.HVArchive(args["max_size"], dimension, rng)
    else:
        print(f"error: undefined archive type: {archive_type}")
        sys.exit(1)

    # Add solutions to archive
    iteration = 0
    for sol in solutions:
        iteration += 1
        archive.add(sol)
        if args["fileprefix"]:
            fname = f"{args['fileprefix']}.{iteration}"
            with open(fname, "w") as fp:
                for s in archive.get_solutions():
                    fp.write(" ".join(map(str, s)) + "\n")
        if args["seq_length"] and iteration >= args["seq_length"]:
            break

    if not args["fileprefix"]:
        for s in archive.get_solutions():
            print(" ".join(map(str, s.get_objectives())))

if __name__ == "__main__":
    main()