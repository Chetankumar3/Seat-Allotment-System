# Allocation Engine

This Allocation Engine implements the IIIT Naya Raipur state-quota counselling logic in C++. It reads applicants and the seat matrix from CSV files, sorts by rank, and runs allotment in repeated passes.

The code is intentionally explicit. It keeps the quota state directly in memory and updates it after each vacancy transfer.

## Input Files

The program expects these files in the working directory:

- `Cleaned Applicants.csv` - applicant master list.
- `new_seat_matrix.csv` - branch-wise seat matrix.
- `allotted.csv` - output file written by the program.

The CSV parsing in `Final_Logic.cpp` assumes a simple comma-separated layout without quoted commas inside fields.

## High-Level Flow

1. Read applicants and sort them by merit rank.
2. Read the seat matrix and build branch/category/quota seat pools.
3. Run the first allotment pass.
4. Apply the manual document-based vacancy conversions and evictions.
5. Re-run the engine to update allotments after each conversion.
6. Merge the final allotments and write `allotted.csv`.

Allocation is not a single scan. It is a sequence of scans over the same rank-ordered applicant list, with seat pools changing between scans.

## Quota Model

The logic uses three layers of reservation state:

### 1) Vertical reservation

These are the category buckets:

- `UR`
- `SC`
- `ST`
- `OBC`
- `EWS` for NTPC seats

Vertical reservation decides the base category pool.

### 2) Horizontal reservation

These are applied inside a vertical category:

- `OPEN`
- `F` - female
- `EXG` - ex-servicemen / ex-guard-like horizontal bucket used by the source data
- `FF` - freedom fighter
- `PWD` - persons with disability

Horizontal reservation is a sub-bucket inside each category bucket.

### 3) Nested or combined reservations

The document also has combined cases such as `F+EXG`, `F+FF`, and `F+PWD`. The implementation handles these by priority order, not by separate seat types.

Example:

- `SC(F+EXG)` means the applicant can be considered against `SC(F)`, `SC(EXG)`, `SC(OPEN)`, and the converted `UR` equivalents, in the order defined by the logic table.

This is why the code stores rule tables instead of hard-coding each case.

## Data Structures In The Code

### `Applicant`

Each applicant stores:

- identity fields such as serial number, application number, name, email, and rank
- gender and category flags
- quota flags for `PWD`, `EXG`, `FF`, and `OPEN`
- three branch preferences
- the final allotment result

Applicants are sorted by rank before any seat allocation begins.

### Seat pools

The main seat state is kept in:

- `CHSeats` for Chhattisgarh quota seats
- `NTPCSeats` for NTPC quota seats

Each is indexed as:

- branch index `0..2`
- category key such as `UR`, `SC`, `ST`, `OBC`, `EWS`
- quota key such as `OPEN`, `F`, `EXG`, `FF`, `PWD`

Each leaf stores a `multiset<string>` of seat labels. A multiset is used because:

- seats are added and removed repeatedly across iterations
- seat labels need to be preserved when seats are released and merged into another pool
- the structure makes vacancy transfer easy and deterministic

### Logic tables

The program uses two rule maps:

- `LogicLines` for Chhattisgarh quota
- `NTPCLogicLines` for NTPC quota

These maps encode the allowed fallback order for each combination of:

- category
- gender
- horizontal quota

They are the programmatic version of the logic table from the counselling document.

### Kashmiri migrant seats

`KashmiriSeats` is handled separately because it is branch-specific and not stored in the same category/quota pool as the other seats.

## Allotment Rules As Implemented

The core routine is `AllotChoice`.

For a candidate, it tries to allocate a preferred branch against a seat pool determined by:

- the applicant’s applied quota
- the current iteration number
- the applicant’s category and horizontal reservation flags

### Chhattisgarh quota applicants

For quota `1`, the code first tries the exact quota pool and then falls back through the rule table for the current iteration.

### NTPC quota applicants

For quota `2`, the code follows the NTPC-specific rule table. The NTPC logic is slightly different because the document treats `EWS` and some horizontal conversions separately.

### Kashmiri migrant applicants

For quota `3`, the candidate is handled branch-wise. If a branch has a Kashmiri seat available, the seat is assigned directly. The branch preference order still matters.

## Iterative Vacancy Conversion

The allocation is intentionally repeated after each vacancy conversion step. This matches the counselling flow: fill, evict or convert vacant seats as prescribed by the manual document check, then re-run to update the allotments.

The sequence in `MainLogic()` is:

1. Initial allocation pass.
2. Convert unused horizontal seats to `OPEN` in the Chhattisgarh pools.
3. Convert unused horizontal seats to `OPEN` in the NTPC pools, except `F` seats which stay in their own female bucket.
4. Run allocation again.
5. Convert `SC` vacancies into `ST` pools.
6. Run allocation again.
7. Convert `ST` vacancies back into `SC` pools.
8. Run allocation again.
9. Repeat the `SC` to `ST` conversion once more.
10. Run allocation again.
11. Convert `ST` vacancies to `OBC` pools.
12. Run allocation again.
13. Convert `OBC` vacancies to `UR` pools.
14. Run allocation again.
15. Convert remaining NTPC `EWS` vacancies to `UR` pools.
16. Run the final allocation pass.

The important behavior is that each conversion step changes the live seat pools, and the next pass re-evaluates all remaining unallotted applicants against the newly available seats.

## What The Output Means

`allotted.csv` contains the final assigned applicants in merit order, with these columns:

- serial number
- application number
- name
- email
- rank
- branch
- allotment description

The allotment description includes both the original seat label and the way it was reached, for example when a seat moves from one quota bucket to another during vacancy conversion.

## Notes On The Implementation

- The code is written to make state transitions explicit.
- The allocation logic is driven by the source document’s quota order, not by a generic matching engine.
- Branches are independent seat pools, but the same applicant may move between pools across iterations if a better or later-converted seat becomes available.
- This README documents the engine as it exists in `Final_Logic.cpp`.

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
