# Evaluation

This folder contains the results of our evaluation and the scripts used to elaborate the results.

## Folder structure
```
├── plots
│   └── experiments.pdf
├── raw_data
│   ├── experiments
│   ├── microbenchmark
│   └── base.csv
├── tables
├── README.md
├── requirements.txt
├── experiments.py
└── microbenchmark.py
```

## Raw data
The *raw_data* folder contains logs of the *UART* messages from the board.

The *base.csv* file contains the base evaluation of general parameters.

The *microbenchmark* sub-folder contains data collected about the overhead of the single operations of SAVE and RESTORE, conducted for an application size (excluding the size of the checkpoint utility) of 2048 bytes.
The experiment is repeated considering the application's data as either entirely confidential or entirely non-confidential.
The application's size was simulated by a buffer inside the .bss section, which was marked either confidential or non-confidential.

The *experiments* sub-folder contains data about the overall overhead of SAVE and RESTORE, collected for different application sizes.
The application's data is considered either entirely confidential or entirely non-confidential, depending on the experiment.
Also in this case, the application's size was simulated by a buffer inside the .bss section, which was marked either confidential or non-confidential.

## Scripts
The *microbenchmark.py* script analyzes the microbenchmark data and generates summary tables inside the *tables* folder.
The *experiments.py* script analyzes the different experiments and generates a summary table inside the *tables* folder and its graphical representation inside the *plots* folder.

Before running the scripts, create a python virtual environment (preferred) and install the required libraries (reported in *requirements.txt*).
```bash
python -m venv .venv
source .venv/bin/activate # (on linux)
python -m pip install -r requirements.txt
```

The scripts are divided in cells that can be run in VSCode notebooks by pressing *Shift + Enter* when selected.
Everything was tested with *Python 3.10.12*





