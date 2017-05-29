from subprocess import Popen, PIPE, call, DEVNULL
import os

adminExecutable = './bin/admin'
serverExecutable = './bin/server'
workerExecutable = './bin/worker'

def runServer(*args):
    return Popen([serverExecutable,*args], stdout=DEVNULL, stderr=DEVNULL)

def runWorker(*args):
    return Popen([workerExecutable,*args], stdout=DEVNULL, stderr=DEVNULL)

def test1():
    inst = []
    inst.append(runServer())
    inst.append(runWorker())
    inst.append(runWorker())
    inst.append(runWorker())

    p = Popen([adminExecutable, 'localhost', '-l', '-d'],stdout=PIPE)
    out = p.communicate()
    out = out[0].decode()

    expectedOut='''
0 -> ::1
1 -> ::1
2 -> ::1
'''

    for i in inst:
        i.kill()

    lines1 = [l.strip() for l in out.strip().split('\n')         if l.strip()!='']
    lines2 = [l.strip() for l in expectedOut.strip().split('\n') if l.strip()!='']
    return lines1 == lines2

if __name__ == "__main__":
    print("running tests")
    tests = [test1]
    for i,t in enumerate(tests):
        if not t():
            print("test",i,"failed")
        else:
            print("test",i,"passed")
