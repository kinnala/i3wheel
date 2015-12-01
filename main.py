import i3ipc
import subprocess
import time
from evdev import InputDevice, list_devices, categorize, ecodes

# list all input devices
devices=[InputDevice(fn) for fn in list_devices()]
for dev in devices:
    print(dev.fn, dev.name, dev.phys)

# set up correct input device
dev=InputDevice('/dev/input/event3')

def startoverlay(workspaces):
    screens={}

    # loop over workspaces and find different screens
    for space in workspaces:
        key=space['output']
        if key not in screens:
            screens[key]=space['rect']
            screens[key]['invisible']=[]
            screens[key]['visible']=0
        # add the numbers of virtual workspaces of each screen
        # to dict(screens)
        if space['visible']:
            screens[key]['visible']=space['num']
        else:
            screens[key]['invisible'].append(space['num'])

    overlayarg=["./overlay"]

    for output,data in screens.iteritems():
        nspaces=len(data['invisible'])+1
        overlayarg.append(str(nspaces))
        overlayarg.append(str(data['x']+data['width']/2))
        overlayarg.append(str(data['y']+data['height']/2))
        if nspaces==1:
            overlayarg.append(str(0))
        else:
            overlayarg.append(str(25+13*nspaces))
        overlayarg.append(str(data['visible']))
        for itr in data['invisible']:
            overlayarg.append(str(itr))
        overlayarg.append("-1")

    overlayarg=overlayarg[:-1]

    return subprocess.Popen(overlayarg)

def main():
    i3=i3ipc.Connection()
    p=0
    moddown=False

    for event in dev.read_loop():
        if event.type==ecodes.EV_KEY:
            # left alt
            if event.code==56:
                if event.value==1:
                    workspaces=i3.get_workspaces()
                    p=startoverlay(workspaces)
                    moddown=True
                elif event.value==0 and p!=0:
                    p.kill()
                    p=0
                    moddown=False
            # other than left alt (-> restart subprocess)
            else:
                if event.value==1:
                    if moddown:
                        p.kill()
                        # restart only if changing workspaces
                        time.sleep(0.1)
                        workspaces=i3.get_workspaces()
                        p=startoverlay(workspaces)


if __name__ == '__main__':
    main()
