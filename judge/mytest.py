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

with open('re_result.csv', newline='') as csvfile:

    # 讀取 CSV 檔案內容
    rows = csv.reader(csvfile)
    datas = list(rows)

    # 以迴圈輸出每一列
    for std in datas[0]:
        if std != "" and std != " bot1" and std != " bot2" and std != " bot3" and std != " 110703029":
            all_std.append(std.strip(" "))

for rstd in re_std:
    PA, PB = 0, 0
    for std in all_std:
        if rstd != std:
            with open('../re_result/A_{}_B_{}.log'.format(rstd, std)) as f:
                mylog = f.read().split("\n")
                for li in range(len(mylog)-1, 0, -1):
                    if mylog[li].find("Win") != -1:
                        if mylog[li].find("A") != -1:
                            PA += 1
                            break
                        elif mylog[li].find("B") != -1:
                            break
                    if mylog[li].find("Tie") != -1:
                        break
            with open('../re_result/A_{}_B_{}.log'.format(std, rstd)) as f:
                mylog = f.read().split("\n")
                for li in range(len(mylog)-1, 0, -1):
                    if mylog[li].find("Win") != -1:
                        if mylog[li].find("A") != -1:
                            break
                        elif mylog[li].find("B") != -1:
                            PB += 1
                            break
                    if mylog[li].find("Tie") != -1:
                        break
    print("{}, {}, {}".format(rstd, round(PA/63, 2), round(PB/63, 2)))

108208117, 0.37, 0.06
109208064, 0.87, 0.87
110703021, 0.86, 0.81
110703038, 0.89, 0.73
110703047, 0.75, 0.68
110703051, 0.84, 0.70
110703061, 0.38, 0.30