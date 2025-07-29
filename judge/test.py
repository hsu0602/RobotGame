import csv

all_std = []
re_std = [
    '108208117',
    '109208064',
    '110703021',
    '110703038',
    '110703047',
    '110703051',
    '110703061'
]

BS = [
    0, 15, 30, 40, 50, 60, 70
]

with open('test.csv', 'w') as f:
    f.write(" , Bot, PA, PB\n")

with open('re_result.csv', newline='') as csvfile:

    # 讀取 CSV 檔案內容
    rows = csv.reader(csvfile)
    datas = list(rows)

    # 以迴圈輸出每一列
    for std in datas[0]:
        if std != "" and std != " bot1" and std != " bot2" and std != " bot3" and std != " 110703029":
            all_std.append(std.strip(" "))

    for i in range(len(datas)):
        std = datas[i][0].strip(" ")
        if std in re_std:
            bot, PA, PB = 0, 0, 0
            # 1
            if datas[1][i].strip(" ") == std:
                bot += 1
            if datas[i][1].strip(" ") == std:
                bot += 1
            # 2
            if datas[2][i].strip(" ") == std:
                bot += 1
            if datas[i][2].strip(" ") == std:
                bot += 1
            # 3
            if datas[3][i].strip(" ") == std:
                bot += 1
            if datas[i][3].strip(" ") == std:
                bot += 1
            # P
            print("====", std)
            for j in range(4, len(datas)):
                oppo = datas[0][j].strip(" ")
                if i != j and oppo != "" and oppo != "bot1" and oppo != "bot2" and oppo != "bot3" and oppo != "110703029":                        
                    if datas[i][j].strip(" ") != oppo and datas[i][j].strip(" ") != "Tie" and datas[i][j].strip(" ") != "-":
                        PA += 1
                    if datas[j][i].strip(" ") == std:
                        PB += 1
            print()

            with open('test.csv', 'a') as f:
                f.write("{}, {}, {}, {}\n".format(std, BS[bot], PA, PB))

