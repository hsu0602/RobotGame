from os import listdir
import subprocess

students_path = "../students"
students = []
bots = ["bot1", "bot2", "bot3"]
files = listdir(students_path)

for f in files:
    students.append(f.strip(".cpp"))
students = sorted(students)

# init
result_list = ["", "bot1", "bot2", "bot3"]
result_table = [["-" for _ in range(len(students)+3)] for _ in range(len(students)+3)]
std_index = {}
std_index["bot1"] = 0
std_index["bot2"] = 1
std_index["bot3"] = 2
for i in range(len(students)):
    result_list.append(students[i])
    std_index[students[i]] = i + 3

# make csv
with open('result.csv', 'w') as f:
    for i in range(len(result_list)-1):
        f.write(result_list[i] + ", ")
    f.write(result_list[-1] + "\n")

for std in students:
    for bot in bots:
        print("judging {} & {}".format(std, bot))
        subprocess.run("cd .. && rm -rf tmp", shell=True)
        subprocess.run("cp ../students/{}.cpp ../player/player.cpp".format(std), shell=True)
        if bot == "bot1":
            subprocess.run("cd .. && make judge1", shell=True)
        elif bot == "bot2":
            subprocess.run("cd .. && make judge2", shell=True)
        elif bot == "bot3":
            subprocess.run("cd .. && make judge3", shell=True)
        try:
            with open('../move_player_be_A.log') as f:
                mylog = f.read().split("\n")
                for i in range(len(mylog)-1, 0, -1):
                    if mylog[i].find("Win") != -1:
                        if mylog[i].find("A") != -1:
                            result_table[std_index[std]][std_index[bot]] = std
                            break
                        elif mylog[i].find("B") != -1:
                            result_table[std_index[std]][std_index[bot]] = bot
                            break
                    if mylog[i].find("Tie") != -1:
                        result_table[std_index[std]][std_index[bot]] = "Tie"
                        break
            with open('../move_player_be_B.log') as f:
                mylog = f.read().split("\n")
                for i in range(len(mylog)-1, 0, -1):
                    if mylog[i].find("Win") != -1:
                        if mylog[i].find("A") != -1:
                            result_table[std_index[bot]][std_index[std]] = bot
                            break
                        elif mylog[i].find("B") != -1:
                            result_table[std_index[bot]][std_index[std]] = std
                            break
                    if mylog[i].find("Tie") != -1:
                        result_table[std_index[bot]][std_index[std]] = "Tie"
                        break
            subprocess.run("mv ../move_player_be_A.log ../result/A_{}_B_{}.log".format(std, bot), shell=True)
            subprocess.run("mv ../move_player_be_B.log ../result/A_{}_B_{}.log".format(bot, std), shell=True)
        except:
            pass

for i in range(len(students)):
    for j in range(i+1, len(students)):
        print("judging {} & {}".format(students[i], students[j]))
        subprocess.run("cd .. && rm -rf tmp", shell=True)
        subprocess.run("cp ../students/{}.cpp ../player/player.cpp".format(students[i]), shell=True)
        subprocess.run("cp ../students/{}.cpp ../player/player2.cpp".format(students[j]), shell=True)
        subprocess.run("cd .. && make judge", shell=True)
        try:
            with open('../move_player_be_A.log') as f:
                mylog = f.read().split("\n")
                for li in range(len(mylog)-1, 0, -1):
                    if mylog[li].find("Win") != -1:
                        if mylog[li].find("A") != -1:
                            result_table[std_index[students[i]]][std_index[students[j]]] = students[i]
                            break
                        elif mylog[li].find("B") != -1:
                            result_table[std_index[students[i]]][std_index[students[j]]] = students[j]
                            break
                    if mylog[li].find("Tie") != -1:
                        result_table[std_index[students[i]]][std_index[students[j]]] = "Tie"
                        break
            with open('../move_player_be_B.log') as f:
                mylog = f.read().split("\n")
                for li in range(len(mylog)-1, 0, -1):
                    if mylog[li].find("Win") != -1:
                        if mylog[li].find("A") != -1:
                            result_table[std_index[students[j]]][std_index[students[i]]] = students[j]
                            break
                        elif mylog[li].find("B") != -1:
                            result_table[std_index[students[j]]][std_index[students[i]]] = students[i]
                            break
                    if mylog[li].find("Tie") != -1:
                        result_table[std_index[students[j]]][std_index[students[i]]] = "Tie"
                        break
            subprocess.run("mv ../move_player_be_A.log ../result/A_{}_B_{}.log".format(students[i], students[j]), shell=True)
            subprocess.run("mv ../move_player_be_B.log ../result/A_{}_B_{}.log".format(students[j], students[i]), shell=True)
        except:
            pass

with open('result.csv', 'a') as f:
    for i in range(1, len(result_list)):
        f.write(result_list[i] + ", ")
        for j in range(1, len(result_list)-1):
            f.write(result_table[i-1][j-1] + ", ")
        f.write(result_table[i-1][-1] + "\n")