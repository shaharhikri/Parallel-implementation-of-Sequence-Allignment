def printDictAsCarr(d: dict):
    print('{',end='')
    for c in range(ord('A'),ord('Z')+1):
        print('"'+d[chr(c)]+'"',' ,')
    print('};',end='')

def toNotDict(lst: list):
    notD = dict();

    for c in range(ord('A'), ord('Z') + 1):
        notD[chr(c)] = ""

    for c1 in range(ord('A'), ord('Z') + 1):
        for c2 in range(ord('A'), ord('Z') + 1):
            if c1 != c2:
                flag = True
                for group in lst:
                    if group.__contains__(chr(c2)) and group.__contains__(chr(c1)):
                        #print(chr(c1), chr(c2), group)
                        flag = False
                        break
                if flag == True:
                    notD[chr(c1)] += chr(c2)
    return notD

def toDict(lst: list):
    D = dict();

    for c in range(ord('A'), ord('Z') + 1):
        D[chr(c)] = ""

    for c1 in range(ord('A'), ord('Z') + 1):
        for c2 in range(ord('A'), ord('Z') + 1):
            if c1 != c2:
                flag = True
                for group in lst:
                    if group.__contains__(chr(c2)) and group.__contains__(chr(c1)):
                        #print(chr(c1), chr(c2), group)
                        flag = False
                        break
                if flag == False:
                    D[chr(c1)] += chr(c2)
    return D

def dict2groupsIntersection(dict1: dict,in_dict1: bool,dict2: dict,in_dict2: bool):
    inter_d = dict()

    for c in range(ord('A'), ord('Z') + 1):
        inter_d[chr(c)] = ""

    for c1 in range(ord('A'), ord('Z') + 1):
        for c2 in range(ord('A'), ord('Z') + 1):
            if c1 != c2:

                if ( (in_dict1 and dict1[chr(c1)].__contains__(chr(c2)) ) or (not in_dict1 and not dict1[chr(c1)].__contains__(chr(c2)) ) ) and ( (in_dict2 and dict2[chr(c1)].__contains__(chr(c2))) or (not in_dict2 and not dict2[chr(c1)].__contains__(chr(c2))) ):
                    inter_d[chr(c1)] += chr(c2)

    return inter_d

Con = ["NDEQ","NEQK","STA","MILV","QHRK","NHQK","FYW","HY","MILF"]
Semi = ["SAG","ATV","CSA","SGND","STPA","STNK","NEQHRK","NDEQHK","SNDEQK","HFY","FVLIM"]

Con_d = toDict(Con)
Semi_d = toDict(Semi)
#NotCon_d = toNotDict(Con)
#NotSemi_d = toNotDict(Semi)

#Con_dict
print('char* Con_dict[] = ')
printDictAsCarr(Con_d)
print('\n\n')

#ConAndNotSemi_dict
ConAndNotSemi_d = dict2groupsIntersection(Con_d,True,Semi_d,False)
print('char* ConAndNotSemi_dict[] = ')
printDictAsCarr(ConAndNotSemi_d)
print('\n\n')

#SemiAndNotCon_dict
SemiAndNotCon_d = dict2groupsIntersection(Semi_d,True,Con_d,False)
print('char* SemiAndNotCon_dict[] = ')
printDictAsCarr(SemiAndNotCon_d)
print('\n\n')

#ConAndSemi_dict
ConAndSemi_d = dict2groupsIntersection(Con_d,True,Semi_d,True)
print('char* ConAndSemi_dict[] = ')
printDictAsCarr(ConAndSemi_d)
print('\n\n')

#notSemiAndNotCon_dict
notSemiAndNotCon_d = dict2groupsIntersection(Semi_d,False,Con_d,False)
print('char* notSemiAndNotCon_dict[] = ')
printDictAsCarr(notSemiAndNotCon_d)