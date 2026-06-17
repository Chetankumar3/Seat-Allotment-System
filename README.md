# Allocation Engine

This Allocation Engine implements the IIIT Naya Raipur state-quota counselling logic in C++. It reads applicants and the seat matrix from CSV files, sorts by rank, and runs allotment in repeated passes.

It is intentionally strict and explicit because the seat logic is sensitive to small mistakes.

## Input Files

The program expects these files in the working directory:

- `Cleaned Applicants.csv` - applicant master list.
- `new_seat_matrix.csv` - branch-wise seat matrix.
- `allotted.csv` - output file written by the program.

The CSV parsing in `Final_Logic.cpp` assumes a simple comma-separated layout without quoted commas inside fields.

## What Makes It Complex

- The logic mixes vertical reservation, horizontal reservation, and combined cases like `F+EXG`.
- Seats are not filled once; they are re-run after vacancy conversions and evictions from the manual document rules.
- The same rank-sorted applicant list is scanned multiple times while the seat pools change.
- The implementation is written to avoid ambiguity and keep the allotment path predictable.

## Flow

1. Load applicants and seat matrix.
2. Run allotment.
3. Apply the documented vacancy transfers and evictions.
4. Re-run allotment after each update.
5. Write the final `allotted.csv`.

## Notes

- This is an engine, not a general-purpose allocator.
- The code follows the counseling logic closely to keep the result stable and error-free.

## Build And Run

Compile the program with a C++17-compatible compiler, for example:

```bash
g++ -std=c++17 Final_Logic.cpp -o allotment
```

Then run the executable from the folder that contains the input CSV files.

## Caveats

- The CSV parser is intentionally simple and will not handle embedded commas inside quoted fields.
- The code assumes the seat matrix layout matches the expected column order.
- The vacancy conversion order is hard-coded to match the counselling flow used for this project.
