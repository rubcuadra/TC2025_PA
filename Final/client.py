#!/usr/bin/env python3
import socket
from time import sleep
from enum import Enum
from onitampy.board import OnitamaBoard

#CODES FOR COMMUNICATION
class options():
    PVE = b'0'
    PVP = b'1'
    DIFFICULTY_NORMAL = b'1'
    DIFFICULTY_HARD   = b'2'

class responses():
    OK = '0'

BLUE = 0
RED  = 1

HOST = '127.0.0.1'  # The server's hostname or IP address
PORT = 8989        # The port used by the server

#sends toSend and waits for a response, if it matches expectedAnswer it returns True
def send(s,toSend,expectedAnswer=responses.OK):
    s.sendall(toSend) #PVE or PVP - 1 | 0 
    return receive(s) == expectedAnswer

def receive(s):
    d = s.recv(1024)
    return d.decode('utf-8')[:-1] if d else None#-1 removes \0

if __name__ == '__main__':
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        #Get it from GUI
        mode       = options.PVE 
        difficulty = options.DIFFICULTY_HARD
        table      = 0
        if mode == options.PVE: 
            if send(s,options.PVE): #PVP | PVE
                if send(s,difficulty): #NORMAL
                    ans = receive(s)
                    if ans:
                        ans     = ans.split(" ")
                        we      = int(ans[0]) #0 => BLUE
                        playing = 0
                        board   = OnitamaBoard()
                        board.setCardsById(ans[1:]) 
                        while not board.isGameOver():
                            print(board)
                            if we == playing: 
                                while True:
                                    print(f"YOUR TURN: {'BLUE' if we==0 else 'RED' }")
                                    print("Fr fc tr tc movement")
                                    text = input("> ")
                                    ans = text.split(" ")
                                    if len(ans)==5: 
                                        fr,fc,tr,tc,mov_id = [int(c) for c in ans] #Convert to int
                                        print(board.getCardById(mov_id))
                                        if board.canMove( board.BLUE if we==0 else board.RED, (fr,fc), board.getCardById(mov_id), (tr,tc) ):
                                            if send(s,f"{fr} {fc} {tr} {tc} {mov_id}".encode()): #SEND IT
                                                board = board.move(board.BLUE if we==0 else board.RED, (fr,fc), board.getCardById(mov_id), (tr,tc))
                                                playing = (playing+1)%2
                                                break
                                    print("Wrong movement,try again")    
                            else:
                                fr,fc,tr,tc,mov_id = [int(c) for c in receive(s).split(" ")] #Convert to int
                                # print(fr,fc,tr,tc,mov_id)
                                board = board.move( board.BLUE if we==1 else board.RED , (fr,fc), board.getCardById(mov_id), (tr,tc))
                                playing = (playing+1)%2
        elif mode == options.PVP:
            pass #working on it
        else:
            print("WRONG OPTION, BYE")     
        s.close()
    