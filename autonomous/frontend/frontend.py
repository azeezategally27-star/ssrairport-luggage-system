#!/usr/bin/env python3
"""
Simple Pygame frontend for the autonomous agents backend.
Connects to local server, fetches agent list and renders them in a purple/white UI.
Click an agent to print details and send a simple 'charge' command.
"""
import sys, socket, json, time, argparse
import pygame

parser = argparse.ArgumentParser()
parser.add_argument('--connect', default='127.0.0.1:9292')
args = parser.parse_args()
host,port = args.connect.split(':')
port = int(port)

WIDTH, HEIGHT = 1000, 700
BG = (250,250,253)
PURPLE = (107,66,230)
WHITE = (255,255,255)

pygame.init()
screen = pygame.display.set_mode((WIDTH,HEIGHT))
clock = pygame.time.Clock()
font = pygame.font.SysFont('Arial', 16)

agents = []
selected = None

def fetch_agents():
    try:
        s = socket.create_connection((host,port), timeout=0.5)
        s.sendall(b'GET /agents\n')
        data = s.recv(8192)
        s.close()
        if not data: return []
        arr = json.loads(data.decode('utf-8'))
        return arr
    except Exception as e:
        # print('fetch error', e)
        return []

def send_command(agent_id, cmd):
    try:
        s = socket.create_connection((host,port), timeout=0.5)
        msg = f"CMD {agent_id} {cmd}\n".encode('utf-8')
        s.sendall(msg)
        data = s.recv(256)
        s.close()
        return data.decode('utf-8')
    except Exception as e:
        print('cmd error', e)
        return None

def world_to_screen(x,y):
    # simple transform: center
    sx = WIDTH//2 + int(x*10)
    sy = HEIGHT//2 + int(y*10)
    return sx, sy

running = True
last_fetch = 0
while running:
    for ev in pygame.event.get():
        if ev.type == pygame.QUIT:
            running = False
        elif ev.type == pygame.MOUSEBUTTONDOWN and ev.button == 1:
            mx,my = ev.pos
            # check clicks on agents
            for a in agents:
                sx,sy = world_to_screen(a['x'], a['y'])
                if (mx-sx)**2 + (my-sy)**2 < 20*20:
                    selected = a
                    # send a charge command as example
                    out = send_command(a['id'], 'charge')
                    print('send cmd', out)
                    break

    now = time.time()
    if now - last_fetch > 0.3:
        agents = fetch_agents()
        last_fetch = now

    screen.fill(BG)
    # left panel
    pygame.draw.rect(screen, WHITE, (10,10,320,680))
    pygame.draw.rect(screen, PURPLE, (10,10,320,50))
    title = font.render('Agents - MRU Autonomous', True, WHITE)
    screen.blit(title, (20,20))

    # draw agents in world
    for a in agents:
        sx,sy = world_to_screen(a['x'], a['y'])
        color = (100,200,255) if a['battery']>0.3 else (255,120,120)
        pygame.draw.circle(screen, color, (sx,sy), 12)
        # name
        nm = font.render(a['name'], True, (20,20,20))
        screen.blit(nm, (sx+14, sy-8))

    # right info panel
    pygame.draw.rect(screen, WHITE, (690,10,300,680))
    pygame.draw.rect(screen, PURPLE, (690,10,300,50))
    info_title = font.render('Inspector Panel', True, WHITE)
    screen.blit(info_title, (700,20))

    if selected:
        y0 = 80
        lines = [f"id: {selected['id']}", f"name: {selected['name']}", f"pos: ({selected['x']:.2f},{selected['y']:.2f})", f"battery: {selected['battery']:.2f}", f"task: {selected['task']}"]
        for i,l in enumerate(lines):
            t = font.render(l, True, (30,30,30))
            screen.blit(t, (700, y0 + i*26))

    pygame.display.flip()
    clock.tick(60)

pygame.quit()
