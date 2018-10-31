#!/usr/local/bin/python
import socket
from time import sleep
from enum import Enum
from threading import Thread
from gui import Onitama_GUI
from onitampy.board import OnitamaBoard
from onitampy.movements import MOVEMENT_ID
from time import sleep
from codes import *
from sys import argv

gui = Onitama_GUI(OnitamaBoard())
START_GUI = 0

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 8989        # The port used by the server

#sends toSend and waits for a response, if it matches expectedAnswer it returns True
def send(s,toSend,expectedAnswer=responses.OK):
    s.sendall(toSend) #PVE or PVP - 1 | 0 
    return receive(s) == expectedAnswer

def sendAndReturn(s,toSend): 
    s.sendall(toSend) #PVE or PVP - 1 | 0 
    return receive(s)

def receive(s):
    d = s.recv(1024)
    return d.decode('utf-8')[:-1] if d else None#-1 removes \0

def getModeFromUser():
    print("Select mode:\n\t1)Play Vs Player\n\t2)Play Vs Computer")
    while True:
        a = input("> ")
        if a.isdigit():
            a = int(a)
            if a > 0 and a < 3:
                return options.PVP if a==1 else options.PVE
        print("Wrong option, try again")

def getDifficultyFromUser():
    print("Select difficulty:\n\t1)Normal\n\t2)Hard")
    while True:
        a = input("> ")
        if a.isdigit():
            a = int(a)
            if a > 0 and a < 3:
                return options.DIFFICULTY_NORMAL if a==1 else options.DIFFICULTY_HARD
        print("Wrong option, try again")  

def getTableFromUser():
    print("Select a table id: (-1 for random table, table ids from 0 to 4) ")
    while True:
        a = input("> ")
        if a.isdigit():
            a = int(a)
            if a == -1 or (a >= 0 and a < 5):
                return a
        print("Wrong option, try again")  

class OnitamaClient(Thread):
    def __init__(self):
        ''' Constructor. '''
        Thread.__init__(self)

    def run(self): #START_GUI
        global START_GUI,gui
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            #CONNECTION SUCCESFULL, LAUNCH WINDOW
            mode = getModeFromUser()
            if mode == options.PVE: 
                if send(s,mode): 
                    difficulty = getDifficultyFromUser()
                    if send(s,difficulty): #NORMAL
                        ans = receive(s)
                        if ans:
                            ans       = ans.split(" ")
                            we        = int(ans[0]) #0 => BLUE
                            playing   = 0
                            board = OnitamaBoard()
                            gui.player= board.BLUE if we==0 else board.RED
                            board.setCardsById(ans[1:]) 
                            START_GUI = 1
                            while not board.isGameOver():
                                gui.board = board
                                gui.turn  = board.BLUE if playing==0 else board.RED
                                if we == playing: 
                                    while True:
                                        ans = gui.getSelectedMovement()
                                        while ans == None: 
                                            sleep(0.25)
                                            ans = gui.getSelectedMovement()
                                        gui.resetMovement()
                                        fr,fc  = ans[0]
                                        mov_id = MOVEMENT_ID[ans[1]]
                                        tr,tc  = ans[2]
                                        if board.canMove( board.BLUE if we==0 else board.RED, (fr,fc), ans[1], (tr,tc) ):
                                            toS = f"{fr} {fc} {tr} {tc} {mov_id}".encode()
                                            print("SENDING",toS)
                                            if send(s,toS): #SEND IT
                                                board = board.move(board.BLUE if we==0 else board.RED, (fr,fc), board.getCardById(mov_id), (tr,tc))
                                                playing = (playing+1)%2
                                                break
                                        print("Wrong movement,try again")    
                                else:
                                    fr,fc,tr,tc,mov_id = [int(c) for c in receive(s).split(" ")] #Convert to int
                                    board = board.move( board.BLUE if we==1 else board.RED , (fr,fc), board.getCardById(mov_id), (tr,tc))
                                    playing = (playing+1)%2
            elif mode == options.PVP:
                while True:
                    if send(s,mode): 
                        table = str(getTableFromUser())
                        ans = sendAndReturn(s,table.encode()) #Num Players or ERROR (numeric)
                        while ans.isdigit() and int(ans) == 1: #Just 1 player in table, wait until server sends us something different
                            print(ans)
                            ans = receive(s)
                        print(ans)

                        if (ans.isdigit() and int(ans) == 2) or (' ' in ans): 
                            # GAME STARTS
                            if ans.isdigit():
                                ans = receive(s) #GET COLOR AND BOARD
                            if ans:
                                #Initial SETUP
                                ans        = ans.replace("\x000","").split(" ")
                                we         = int(ans[0]) #0 => BLUE
                                playing    = 0
                                board      = OnitamaBoard()
                                board.setCardsById(ans[1:]) 
                                #START
                                gui.board = board
                                gui.player = board.BLUE if we==0 else board.RED
                                START_GUI = 1
                                while not board.isGameOver():
                                    print(board) #TODO draw instead of print

                                    gui.turn = board.BLUE if playing==0 else board.RED
                                    if we == playing: 
                                        while True:
                                            ans = gui.getSelectedMovement()
                                            while ans == None: 
                                                sleep(0.25)
                                                ans = gui.getSelectedMovement()
                                            gui.resetMovement()
                                            fr,fc = ans[0]
                                            mov_id= MOVEMENT_ID[ans[1]]
                                            tr,tc = ans[2]
                                            if board.canMove( board.BLUE if we==0 else board.RED, (fr,fc), board.getCardById(mov_id), (tr,tc) ):
                                                if send(s,f"{fr} {fc} {tr} {tc} {mov_id}".encode()): #SEND IT
                                                    board = board.move(board.BLUE if we==0 else board.RED, (fr,fc), board.getCardById(mov_id), (tr,tc))
                                                    playing = (playing+1)%2
                                                    break
                                        print("Wrong movement,try again")    
                                    else: #TODO si se desconecta aqui truena, debemos cachar y decir que se fue
                                        fr,fc,tr,tc,mov_id = [int(c) for c in receive(s).split(" ")] #Convert to int
                                        board = board.move( board.BLUE if we==1 else board.RED , (fr,fc), board.getCardById(mov_id), (tr,tc))
                                        playing = (playing+1)%2
                            else:
                                raise Exception("ERROR PLAYING")
                            
                        else: #ERROR
                            print(ans, "Algo salio mal")
            else:
                print("WRONG OPTION, BYE")     
            s.close()

if __name__ == '__main__':

    if len(argv) < 3: print("ERROR, execute as\n\t./client {host_address} {port_number}\n")
    else:
        HOST = argv[1]
        PORT = int(argv[2])
        oc = OnitamaClient()
        oc.setName('COMM Thread')
        oc.start()
        while START_GUI == 0: sleep(0.25) #Wait until COMM Thread tells us to launch it
        if START_GUI == 1:    
            print("STARTING GUI")
            gui.run()   #Main Thread will be locked here
        oc.join()
        