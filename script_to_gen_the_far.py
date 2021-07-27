
# import required module
import os
# assign directory
directory = 'algo-master'

di = 'C:\\Users\\Aqib Mahmud\\OneDrive\\Desktop\\tourist\\algo-master'
 
# iterate over files in
# that directory
with open('rime.txt', 'w'):
  for folder in os.listdir(di):
    # print(folder)
    print()
    for filename in os.listdir(f'{directory}\\{folder}'):
        f = os.path.join(directory, filename)
        # checking if it is a file
        # if os.path.isfile(f):
        #     print(f)
        k = os.path.splitext(filename)[0]

        s = f"""<Expand Name="{k}"  At="&OutWord;" SubMenu="1" Pattern="{k}" Imm="-" To="\i'$\\templates\\cp_templates\\{directory}\\{folder}\\{k}.cpp'"/> """
        print(s)
