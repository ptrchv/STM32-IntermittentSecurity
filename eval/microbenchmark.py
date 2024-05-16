# %% import libraries
import pandas as pd

# %% ########## SELECT MICROBENCHMARK FILE ################
MICROBENCH_FILE = "microbench_2048.csv"
# MICROBENCH_FILE = "microbench_2048_conf.csv"

# %% decoding table for msg numbers
code_to_msg = {
    # GENERAL (bytes)
    2: "TM_G_SIZE_DATA",
    3: "TM_G_SIZE_BSS",
    5: "TM_G_SIZE_DATACONF",
    6: "TM_G_SIZE_BSSCONF",

    # SAVE (clock cycles)
    101: "TM_S_TOT",
    102: "TM_S_GEN_IV",

    103: "TM_S_SPI_DATA",		
    104: "TM_S_SPI_BSS",			
    105: "TM_S_SPI_STACK",		
    106: "TM_S_SPI_CRYPTO",			
    107: "TM_S_SPI_TAGS",	

    108: "TM_S_AUTH_DATA",			
    109: "TM_S_AUTH_BSS",		
    110: "TM_S_AUTH_STACK",

    111: "TM_S_CPY_DRIVERS",
    112: "TM_S_FLASH_NONCE",		

    # RESTORE (clock cycles)
    201: "TM_R_TOT1",
    202: "TM_R_TOT2",    

    203: "TM_R_DECRYPT_KEY",
    204: "TM_R_PAGE_WIPE",
    205: "TM_R_FLASH_NONCE",

    206: "TM_R_SPI_DATA",
    207: "TM_R_SPI_BSS",
    208: "TM_R_SPI_STACK",	
    209: "TM_R_SPI_CRYPTO",
    210: "TM_R_SPI_TAGS",

    211: "TM_R_AUTH_DATA",
    212: "TM_R_AUTH_BSS",
    213: "TM_R_AUTH_STACK",

    214: "TM_R_CPY_DRIVERS",
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

# %% confidential and non-confidential memory size for checkpoint utility itself
min_size = df_base.at["TM_G_SIZE_DATA", "val"] + df_base.at["TM_G_SIZE_BSS", "val"]
min_size_conf =  df_base.at["TM_G_SIZE_DATACONF", "val"] + df_base.at["TM_G_SIZE_BSSCONF", "val"]
print(tm_decrypt, tm_page_wipe, min_size, min_size_conf)

# %% load microbenchmak data
df = pd.read_csv("raw_data/microbenchmark/{}".format(MICROBENCH_FILE), header=None)
df.columns = ["code", "val"]
df["code"] = df["code"].apply(lambda x: code_to_msg[x])
#df

# %% average measurements of same parameter
df = df.groupby(["code"]).mean().reset_index()
df["val"] = df["val"].round(0).astype(int)
#df

# %% group contributions by tipology (SPI, Crypto unit, ecc...) and operation (GERAL (S), SAVE (S) / RESTORE (R))
df["code"] = df["code"].apply(lambda x: "TM_S_SPI" if "TM_S_SPI" in x else x)
df["code"] = df["code"].apply(lambda x: "TM_R_SPI" if "TM_R_SPI" in x else x)
df["code"] = df["code"].apply(lambda x: "TM_S_AUTH" if "TM_S_AUTH" in x else x)
df["code"] = df["code"].apply(lambda x: "TM_R_AUTH" if "TM_R_AUTH" in x else x)
df = df.groupby(["code"]).sum().reset_index()
df = df.set_index("code")
df

# %% add corrected key sharing time and page wipe time (divide for checkpoints in page)
df.at["TM_R_PAGE_WIPE", "val"] = tm_page_wipe
df.at["TM_R_DECRYPT_KEY", "val"] = tm_decrypt
df.at["TM_S_PAGE_WIPE", "val"] = tm_page_wipe
df

#%% compute time (ms) overhead give an 160 MHz frequency
df["time_ms"] = df["val"] / (16*10**6) *1000
df

# %% separate by operation (GERAL (S), SAVE (S) / RESTORE (R)) and remove time for GENERAL (they are size measurements)
df_general = pd.DataFrame(df[df.index.str.startswith("TM_G")])
df_save =  pd.DataFrame(df[df.index.str.startswith("TM_S")])
df_restore =  pd.DataFrame(df[df.index.str.startswith("TM_R")])
df_general =  pd.DataFrame(df_general.drop(columns=["time_ms"]))

# %% round clock cycles to integer (RESTORE)
df_restore["val"] = df_restore["val"].round(0).astype(int)
df_restore

# %% round clock cycles to integer (SAVE)
df_save["val"] = df_save["val"].round(0).astype(int)
df_save

# %% remove checkpoint routine size to compute application size (bytes)
df_general.at["TM_G_SIZE", "val"] = df_general.at["TM_G_SIZE_BSS", "val"] + df_general.at["TM_G_SIZE_DATA", "val"] - min_size
df_general.at["TM_G_SIZECONF", "val"] = df_general.at["TM_G_SIZE_BSSCONF", "val"] + df_general.at["TM_G_SIZE_DATACONF", "val"] - min_size_conf
df_general["val"] = df_general["val"].round(0).astype(int)
df_general

# %% save tables
df_restore.to_csv("tables/{}_restore.csv".format(MICROBENCH_FILE.replace(".csv", "")))
df_save.to_csv("tables/{}_save.csv".format(MICROBENCH_FILE.replace(".csv", "")))
df_general.to_csv("tables/{}_general.csv".format(MICROBENCH_FILE.replace(".csv", "")))

# %%
