import pandas as pd

input_df = pd.read_csv("infile.csv")
output_df = pd.read_csv("outfile.csv")

rows_checked = len(output_df)

print(f"Output rows to be validated: {rows_checked}")
num_sorted_items = 5

previous_time = 0
placeholder_time = 0
for index, row in output_df.iterrows():
    if index == 0:
        placeholder_time = row.time
    elif index % num_sorted_items == 0:
        previous_time = placeholder_time
        placeholder_time = row.time

    filtered = input_df[
        (previous_time < input_df.time)
        & (input_df.time <= row.time)
        & (input_df.id == row.id)
    ]
    if not filtered.empty:
        print(
            f"Correct sum for id {row.id} is {filtered.n_cars.sum():02d}, in and out values match: {filtered.n_cars.sum()==row.n_cars}"
        )
