#!/usr/bin/env python
# -*- coding: utf-8 -*-
    
try:
    # for Python2
    from Tkinter import *
except ImportError:
    # for Python3
    from tkinter import *
import tkFileDialog 
from operator import truediv

#from pylab import *
import matplotlib.pyplot as plt


mainWindow = Tk()
mainWindow.title("Passwords Analysis")

typeRegex = StringVar()
regexValue = StringVar()
version = StringVar()
fileName = StringVar()
savePasswords = StringVar()
saveFile = StringVar()
XFirstPasswords = IntVar()


resultArea = Frame(mainWindow, width=400, height=400)
resultArea.pack(side=RIGHT,fill=BOTH, expand=1)
listeResult = Listbox(resultArea, width=50, height=30, activestyle="none")

paramArea = Frame(mainWindow, width=200, height=100, borderwidth=2, relief=GROOVE)
paramArea.pack(side=TOP, fill=NONE, expand=1)

actionArea = Frame(mainWindow, width=300, height=50, bg="orange",borderwidth=2)
actionArea.pack(side=TOP, fill=NONE, expand=1)


personnal = Radiobutton(paramArea, text="Personnal Regex", variable=typeRegex, value="personnal")
predefined = Radiobutton(paramArea, text="Predefined Regex", variable=typeRegex, value="predefined")
entreeRegex = Entry(paramArea, textvariable=regexValue)
regexList = Listbox(paramArea, width=1, height=1, activestyle="dotbox")

fileButton = Button(paramArea, text='Fichier...')
filePath = Entry(paramArea, textvariable=fileName)

withcount = Radiobutton(paramArea, text="Version with count", variable=version, value="withcount")
vanilla = Radiobutton(paramArea, text="Version Vanilla", variable=version, value="vanilla")

savePasswordButton = Checkbutton(paramArea, text="Save passwords in file :", variable=savePasswords, onvalue="Yes", offvalue="No")
savePath = Entry(paramArea, textvariable=saveFile)

XFirstPasswordsButton = Button(paramArea, text="Show 'X' firsts passwords :")
numberPasswords = Entry(paramArea, textvariable=XFirstPasswords)

launchButton = Button(actionArea, text="Launch Search")
passwordsSizeButton = Button(actionArea, text="Compare passwords size")
checkAllCharactersButton = Button(actionArea, text="Check all characters")
clearButton = Button(actionArea, text="Clear")


def getRegex():
    if (typeRegex.get() == "predefined"):
        index = regexList.index('active')
        regex = regexList.get(index)
    else:
        regex = regexValue.get()
    return regex

def launchSearch():    
    regex = getRegex()    
    f = open(fileName.get(), "r")
    result = 0
    total = 0
    if (savePasswords.get() == "Yes"):
        wfile = open(saveFile.get()+".txt", "w")

    for line in f:
        line = line.rstrip('\n\r') #passwords without "\n" or "\r"
        process=line.split()
        if (version.get() == "withcount"):
            if len(process) == 2:
                if re.compile(regex).search(process[1]):
                    result = result + int(process[0])
                    if (savePasswords.get() == "Yes"):
                        wfile.write(process[1])
                        wfile.write("\n")
                total = total + int(process[0])
        elif (version.get() == "vanilla"):
            if len(process) == 1:
                if re.compile(regex).search(process[0]):
                    result = result + 1
                    if (savePasswords.get() == "Yes"):
                        wfile.write(process[0])
                        wfile.write("\n")
                total = total + 1
    f.close()
    res = "regex: " + regex + " => " + str(result)  +  "/" +str(total) + " passwords (" + str(100*truediv(result,total)) + "%)"
    listeResult.insert(END, res)
    
    if (savePasswords.get() == "Yes"):
        wfile.close()
    return result

def countDifferentPasswords():
    f = open(fileName.get(), "r")
    result = 0
    for line in f:
        line = line.rstrip('\n\r')
        process = line.split()
        if len(process) == 1:
            result = result + 1
    f.close()
    return result

def chooseVersion():
    if(version.get() == "withcount"):
        fileName.set("./rockyou-withcount.txt")
    elif(version.get() == "vanilla"):
        fileName.set("./rockyou.txt")
    
def clear():
    listeResult.delete(0, END)
    
def askopenfile():
    fileName.set(tkFileDialog.askopenfilename(filetypes = {("Text files", "*.txt")} ))

def showXFirstPasswords():
    clear()
    f = open(fileName.get(), "r")
    i = 0
    if (savePasswords.get() == "Yes"):
        wfile = open(saveFile.get()+".txt", "w")
    for line in f:
        line = line.rstrip('\n\r')
        process=line.split()
        if (version.get() == "withcount"):
            if len(process) == 2:
                listeResult.insert(END, process[1])
                if (savePasswords.get() == "Yes"):
                    wfile.write(process[1])
                    wfile.write("\n")
        elif (version.get() == "vanilla"):
            if len(process) == 1:
                listeResult.insert(END, process[0])
                if (savePasswords.get() == "Yes"):
                    wfile.write(process[0])
                    wfile.write("\n")
        i = i+1
        if (i>=XFirstPasswords.get()):
            return 0

def showXSizePasswords(XSizePasswords):
    f = open(fileName.get(), "r")
    if (savePasswords.get() == "Yes"):
        wfile = open(saveFile.get()+".txt", "w")
    result = 0
    total = 0
    for line in f:
        line = line.rstrip('\n\r')
        process=line.split()
        if (version.get() == "withcount"):
            if len(process) == 2:
                if (len(process[1]) == XSizePasswords):
                    result = result + int(process[0])
                    if (savePasswords.get() == "Yes"):
                        wfile.write(process[1])
                        wfile.write("\n")
                total = total + int(process[0])
        elif (version.get() == "vanilla"):
            if len(process) == 1:
                if (len(process[0]) == XSizePasswords):
                    result = result + 1
                    if (savePasswords.get() == "Yes"):
                        wfile.write(process[0])
                        wfile.write("\n")
                total = total + 1
    f.close()
    res = "Passwords of size" + str(XSizePasswords) + ": " + str(result) +  "/" +str(total) + " (" + str(100*truediv(result,total)) + "%)"
    listeResult.insert(END, res)
    
    if (savePasswords.get() == "Yes"):
        wfile.close()
    return result

def comparePasswordsSize():
    f = open(fileName.get(), "r")
    if (savePasswords.get() == "Yes"):
        wfile = open(saveFile.get()+".txt", "w")
    sizeList={"1": 0, "2": 0, "3": 0, "4": 0, "5": 0, "6": 0, "7": 0, "8": 0, "9": 0, "10": 0, "11": 0, "12": 0, "13": 0, "14": 0, "15": 0,  ">15": 0}
    total = 0
    mean = 0
    for line in f:
        line = line.rstrip('\n\r')
        process=line.split()
        if (version.get() == "withcount"):
            if len(process) == 2:
                mean = mean + len(process[1])*int(process[0])
                if (sizeList.has_key(str(len(process[1])))):
                    sizeList[str(len(process[1]))] = sizeList[str(len(process[1]))] + int(process[0])
                else :
                    sizeList[">15"] = sizeList[">15"] + int(process[0])
                total = total + int(process[0])
        elif (version.get() == "vanilla"):
            if len(process) == 1:
                mean = mean + len(process[1])
                if (sizeList.has_key(str(len(process[1])))):
                    sizeList[str(len(process[1]))] = sizeList[str(len(process[1]))] + 1
                else :
                    sizeList[">15"] = sizeList[">15"] + 1
                total = total + 1
    f.close()
    mean = truediv(mean, total)
    res = "meansize : " + str(mean)
    listeResult.insert(END, res)
    #for key, value in sorted(sizeList.items()): # sorted by size
    for key, value in sorted(sizeList.items(), key = lambda column: column[1], reverse=True): # sorted by occurences
        res = "size = " + key + " => " + str(value) +  "/" +str(total) + " (" + str(100*truediv(value,total)) + "%)"
        listeResult.insert(END, res)
        if (savePasswords.get() == "Yes"):
            wfile.write(key + " " + str(value))
            wfile.write("\n")
        if (100 * truediv(value,total) < 1): # <1 too small for the graphic
            del sizeList[key]
    if (savePasswords.get() == "Yes"):
        wfile.write("meansize : " + str(mean))
        wfile.close()
    key = sizeList.keys()
    value = sizeList.values()
    draw(key, value)
    
def checkAllCharacters():
    f = open(fileName.get(), "r")
    if (savePasswords.get() == "Yes"):
        wfile = open(saveFile.get()+".txt", "w")
    characterList={}
    total = 0
    for line in f:
        line = line.rstrip('\n\r')
        process=line.split()
        if (version.get() == "withcount"):
            if len(process) == 2:
                for character in process[1]:
                    if (characterList.has_key(character)):
                        characterList[character] = characterList[character] + int(process[0])
                    else :
                        characterList[character] = int(process[0])
                    total = total + int(process[0])
        elif (version.get() == "vanilla"):
            if len(process) == 1:
                for character in process[0]:
                    if (characterList.has_key(character)):
                        characterList[character] = characterList[character] + 1
                    else :
                        characterList[character] = 1
                    total = total + 1                
    f.close()
    
    #for key, value in sorted(characterList.items()): # sorted by character
    for key, value in sorted(characterList.items(), key = lambda column: column[1], reverse=True): # sorted by occurences
        res = "Character : " + key + " => " + str(value) +  "/" + str(total) + " (" + str(100 * truediv(value,total)) + "%)"
        listeResult.insert(END, res)
        if (savePasswords.get() == "Yes"):
            wfile.write(key + " " + str(value))
            wfile.write("\n")
        if (100 * truediv(value,total) < 0.5): # <0.5 too small for the graphic
            del characterList[key]
    if (savePasswords.get() == "Yes"):
        wfile.close()
    key = characterList.keys()
    value = characterList.values()
    draw(key, value)

def draw(name, data):
    #plt.pie(x, explode=None, labels=None, colors=None, autopct=None, pctdistance=0.6, shadow=False, labeldistance=1.1, startangle=None, radius=None, counterclock=True, wedgeprops=None, textprops=None, center=(0, 0), frame=False, hold=None, data=None)
    plt.pie(data, labels=name)
    plt.axis('equal')
    plt.show()


personnal.grid(row=0, column=0, sticky=E+W)
predefined.grid(row=1, column=0, sticky=E+W)
typeRegex.set("predefined")
entreeRegex.grid(row=0, column=1, sticky=E+W)

regexList.grid(row=1,column=1, sticky=E+W)
with open('predefinedRegex.txt', 'r') as my_file:
    line = my_file.readline()
    while (line):
        regexList.insert(END, line[:-1])
        line = my_file.readline()

fileName.set("./rockyou-withcount.txt")
fileButton.grid(row=2,column=0, sticky=E+W)
fileButton.config(command=askopenfile)
filePath.grid(row=2, column=1, sticky=E+W)

version.set("withcount")
withcount.grid(row=3, column=0, sticky=E+W)
withcount.config(command=chooseVersion)
vanilla.grid(row=3, column=1, sticky=E+W)
vanilla.config(command=chooseVersion)

savePasswords.set("Yes")
savePasswordButton.grid(row=4, column=0, sticky=E+W)
saveFile.set("untitled")
savePath.grid(row=4, column=1, sticky=E+W)

XFirstPasswordsButton.grid(row=5, column=0, sticky=E+W)
XFirstPasswordsButton.config(command = showXFirstPasswords)
XFirstPasswords.set(1)
numberPasswords.grid(row=5, column=1, sticky=E+W)


launchButton.config(command = launchSearch)
launchButton.grid(row=1, column=0, sticky=W+E)
clearButton.config(command = clear)
clearButton.grid(row=1, column=1, sticky=W+E)
passwordsSizeButton.grid(row=0, column=0, sticky=E+W)
passwordsSizeButton.config(command = comparePasswordsSize)
checkAllCharactersButton.grid(row=0, column=1, columnspan=2, sticky=E+W)
checkAllCharactersButton.config(command = checkAllCharacters)
quitButton = Button(actionArea, text="Quit", command=mainWindow.destroy)
quitButton.grid(row=1, column=2, sticky=W+E)


listeResult.pack()

mainWindow.mainloop()
