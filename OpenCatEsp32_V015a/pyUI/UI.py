#!/opt/anaconda3/envs/petoi/bin/python3
# -*- coding: UTF-8 -*-

# Rongzhong Li
# Petoi LLC
# May.22nd, 2022


from FirmwareUploader import *
from SkillComposer import *
from Calibrator import *
from commonVar import *

language = languageList['English']
apps = ['Firmware Uploader', 'Joint Calibrator', 'Skill Composer']  # ,'Task Scheduler']


def txt(key):
    return language.get(key, textEN[key])


class UI:
    def __init__(self):
        global model
        global language
        try:
            with open("./defaultConfig.txt", "r") as f:
                lines = f.readlines()
                lines = [line[:-1] for line in lines]  # remove the '\n' at the end of each line
                self.defaultLan = lines[0]
                model = lines[1]
                self.defaultPath = lines[2]
                self.defaultSwVer = lines[3]
                self.defaultBdVer = lines[4]
                self.defaultMode = lines[5]
                f.close()

        except Exception as e:
            print('Create configuration file')
            self.defaultLan = 'English'
            model = 'Bittle'
            self.defaultPath = './release'
            self.defaultSwVer = '2.0'
            self.defaultBdVer = 'NyBoard_V1_0'
            self.defaultMode = 'Standard'
        #            raise e

        language = languageList[self.defaultLan]

        self.window = Tk()
        self.ready = False

        self.OSname = self.window.call('tk', 'windowingsystem')
        if self.OSname == 'win32':
            self.window.iconbitmap(r'./resources/Petoi.ico')
            self.window.geometry('320x270+100+10')
        else:
            self.window.geometry('+100+10')
            self.backgroundColor = 'gray'

        self.myFont = tkFont.Font(
            family='Times New Roman', size=20, weight='bold')
        self.window.title(txt('uiTitle'))
        self.createMenu()
        bw = 18
        self.modelLabel = Label(self.window, text=model, font=self.myFont)
        self.modelLabel.grid(row=0, column=0, pady=10)
        for i in range(len(apps)):
            Button(self.window, text=txt(apps[i]), font=self.myFont, fg='blue', width=bw, relief='raised',
                   command=lambda app=apps[i]: self.utility(app)).grid(row=1 + i, column=0, padx=10, pady=(0, 10))

        self.ready = True
        self.window.protocol('WM_DELETE_WINDOW', self.on_closing)
        self.window.update()

        self.window.resizable(False, False)
        self.window.mainloop()

    def createMenu(self):
        self.menubar = Menu(self.window, background='#ff8000', foreground='black', activebackground='white',
                            activeforeground='black')
        file = Menu(self.menubar, tearoff=0, background='#ffcc99', foreground='black')
        for key in NaJoints:
            file.add_command(label=key, command=lambda model=key: self.changeModel(model))
        self.menubar.add_cascade(label=txt('Model'), menu=file)

        lan = Menu(self.menubar, tearoff=0)
        for l in languageList:
            lan.add_command(label=languageList[l]['lanOption'], command=lambda lanChoice=l: self.changeLan(lanChoice))
        self.menubar.add_cascade(label=txt('lanMenu'), menu=lan)

        helpMenu = Menu(self.menubar, tearoff=0)
        helpMenu.add_command(label=txt('About'), command=self.showAbout)
        self.menubar.add_cascade(label=txt('Help'), menu=helpMenu)

        self.window.config(menu=self.menubar)

    def changeModel(self, modelName):
        global model
        model = copy.deepcopy(modelName)
        self.modelLabel.configure(text=model)
        print(model)

    def changeLan(self, l):
        global language
        if self.ready and txt('lan') != l:
            self.defaultLan = l
            print(self.defaultLan)
            language = copy.deepcopy(languageList[l])
            self.menubar.destroy()
            self.createMenu()
            self.window.title(txt('uiTitle'))
            for i in range(len(apps)):
                self.window.winfo_children()[1 + i].config(text=txt(apps[i]))

    def saveConfigToFile(self, filename, config):
        print(config)
        f = open(filename, 'w+')
        lines = '\n'.join(config) + '\n'
        f.writelines(lines)
        f.close()

    def utility(self, app):
        configuration = [self.defaultLan, model, self.defaultPath, self.defaultSwVer, self.defaultBdVer,
                         self.defaultMode]
        self.saveConfigToFile('./defaultConfig.txt', configuration)
        self.window.destroy()

        if app == 'Firmware Uploader':
            Uploader(model, language)
        elif app == 'Joint Calibrator':
            Calibrator(model, language)
        elif app == 'Skill Composer':
            SkillComposer(model, language)
        elif app == 'Task Scheduler':
            print('schedule')

    def showAbout(self):
        messagebox.showinfo('Petoi Desktop App',
                            u'Petoi Desktop App\nOpen Source on GitHub\nCopyright © Petoi LLC\nwww.petoi.com')

    def on_closing(self):
        if messagebox.askokcancel(txt('Quit'), txt('Do you want to quit?')):
            configuration = [self.defaultLan, model, self.defaultPath, self.defaultSwVer, self.defaultBdVer,
                             self.defaultMode]
            self.saveConfigToFile('./defaultConfig.txt', configuration)
            self.window.destroy()


if __name__ == '__main__':
    UI()
