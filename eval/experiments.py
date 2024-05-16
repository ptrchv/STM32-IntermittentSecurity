# %% import libraries
import pandas as pd
import matplotlib.pyplot as plt
import os

# %% decoding table for msg numbers
code_to_msg = {
    2: "TM_G_SIZE_DATA",
    3: "TM_G_SIZE_BSS",

    5: "TM_G_SIZE_DATACONF",
    6: "TM_G_SIZE_BSSCONF",

    101: "TM_S_TOT",

    201: "TM_R_TOT1",
    202: "TM_R_TOT2",
    
    203: "TM_R_DECRYPT_KEY",
    204: "TM_R_PAGE_WIPE",
}

# %% load base evaluation
df_base = pd.read_csv("raw_data/base.csv", header=None)
df_base.columns = ["code", "val"]
df_base["code"] = df_base["code"].apply(lambda x: code_to_msg[x])

# %% compute averages of base evaluation
df_base = df_base.groupby(["code"]).mean().reset_index()
df_base["val"] = df_base["val"].round(0).astype(int)
df_base["time_ms"] = df_base["val"] / (16*10**6) *1000
df_base = df_base.set_index("code")
df_base

# %% key sharing time and page wipe time (divide for checkpoints in page) 
tm_decrypt = df_base.at["TM_R_DECRYPT_KEY", "val"]
tm_page_wipe = int(round(df_base.at["TM_R_PAGE_WIPE", "val"] / (8*1024/16),0))
print(tm_decrypt, tm_page_wipe)

# %% read experiment files
files = ["raw_data/experiments/{}".format(f) for f in os.listdir("raw_data/experiments") if f.startswith("experiment")]
#files = [(int(f.split("_")[1].replace(".csv", "")), f) for f in files]
files.sort()
files

#%% load experiments in dictionaries
results = []
for f in files:
    # load file and decode msg numbers
    df = pd.read_csv(f, header=None)
    df.columns = ["code", "val"]
    df["code"] = df["code"].apply(lambda x: code_to_msg[x])

    # average measurements of same parameter
    df = df.groupby(["code"]).mean().reset_index()
    df["val"] = df["val"].round(0).astype(int)

    # convert to dictionary
    df_dict = {}
    for _, row in df.iterrows():    
        df_dict[row["code"]] = row["val"]

    # create summary dictionary for experiment
    result = {}
    result["R_TOT"] = df_dict["TM_R_TOT1"] + df_dict["TM_R_TOT2"]  + tm_decrypt + tm_page_wipe
    result["S_TOT"] = df_dict["TM_S_TOT"] + tm_page_wipe
    result["SIZE"] = df_dict["TM_G_SIZE_DATA"] + df_dict["TM_G_SIZE_BSS"]
    result["SIZE_CONF"] = df_dict["TM_G_SIZE_DATACONF"] + df_dict["TM_G_SIZE_BSSCONF"]
    result["CONF"] = True if "conf" in f else False
    
    # add to experiment list
    results.append(result)
    print(result)

#%% convert list of dictionaries to dataframe
df = pd.DataFrame(results)
df

# %% duplicate base experiment also for confidential (application size = 0)
row = list(df.iloc[0])
row[4] = True
df.loc[len(df)] = row
df

# %% compute checkpoint routine size overhead
min_size = df["SIZE"].min()
min_size_conf = df["SIZE_CONF"].min()
print(min_size)
print(min_size_conf)

# %% remove checkpoint routine size overhead
df["SIZE"] = df["SIZE"] - min_size
df["SIZE_CONF"] = df["SIZE_CONF"] - min_size_conf
df = df.sort_values(['CONF', 'SIZE_CONF', 'SIZE'], ascending=[True, True, True]).reset_index(drop=True)
df

# %% generate plot of experiments
plt.style.use('seaborn-v0_8-paper')

fig, ax = plt.subplots(1,1)

df_plot = df[df["CONF"] == False]
ax.plot(df_plot["SIZE"], df_plot["S_TOT"]/1000, '--o', label = "save")
ax.plot(df_plot["SIZE"], df_plot["R_TOT"]/1000, '--o', label = "restore")

df_plot = df[df["CONF"] == True]
ax.plot(df_plot["SIZE"], df_plot["S_TOT"]/1000, '--o', label = "save (conf)")
ax.plot(df_plot["SIZE"], df_plot["R_TOT"]/1000, '--o', label = "restore (conf)")

df_plot = df[df["CONF"] == False]

ax.set_xticks(df_plot["SIZE"])

ylabels = [str(int(label)) + 'K' for label in ax.get_yticks()]
ax.set_yticks(ax.get_yticks()[1:-1], ylabels[1:-1])

ax2 = ax.twinx()
y1, y2 = ax.get_ylim()
ax2.set_ylim(y1, y2)

ylabels = [str(int(label) * 1000 / (16*10**6) *1000) for label in ax2.get_yticks()]
ax2.set_yticks(ax2.get_yticks()[1:-1],  ylabels[1:-1])

ax.grid(True)

ax.set_xlabel("Size (bytes)", fontsize = 16)
ax.set_ylabel("Clock cycles", fontsize = 16)
ax2.set_ylabel("Time (ms)", fontsize = 16)

ax.tick_params(labelsize = 14)
ax2.tick_params(labelsize = 14)
# print(df[df["CONF"] == False]["SIZE"])
# ax.set_xticklabels(df[df["CONF"] == False]["SIZE"])
ax.legend(fontsize = 14)

# %% save plot of experiments
fig.tight_layout()
fig.savefig("plots/experiments.pdf")

# %% save table of experiments
df.to_csv("tables/experiment_tot.csv", index=False)

# %%
