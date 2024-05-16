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
The *raw_data* folder contains a logs of the *UART* messages from the board.

The *base.csv* file contains the base evaluation of general parameters.

The *microbenchmark* sub-folder contains data collected about the overhead of the single operations of SAVE and RESTORE, conducted for an application size (excluding the size of the checkpoint utility) of 2048 bytes.
The experiment is repeated considering the application's data either entirely confidential or entirely non-confidential.
The application size was simulated by a buffer inside the .bss section, which was marked either confidential or non-confidential.

The *experiments* sub-folder contains data collected about the overall overhead of SAVE and RESTORE, collected for different application sizes.
The application data is considered either entirely confidential or entirely non-confidential, depending on the experiment.
Also in this case the application size was simulated by a buffer inside the .bss section, which was marked either confidential or non-confidential.

## Scripts
The *microbenchmark.py* script analyzes the microbenchmark data and generate summary tables inside the *tables* folder
The *experiments.py* script analyzes the different experiments and generates a summary table inside the *tables* folder its graphical representation inside the *plots* folder.

Before running the scripts create a python virtual environment (preferred) and install the required libraries (reported in *requirements.txt*).
```bash
python -m venv .venv
source .venv/bin/activate # (on linux)
python -m pip install -r requirements.txt
```

The scripts are divided in cells that can be run in VSCode notebooks by pressing *Shift + Enter* in the respective cells.
Everything was tested with *Python 3.10.12*





