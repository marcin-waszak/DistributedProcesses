from subprocess import Popen, PIPE, call
import os

adminExecutable = './bin/admin'
serverExecutable = './bin/server'
workerExecutable = './bin/worker'

def runServer(*args):
    with open(os.devnull, "w") as f:
        return Popen([serverExecutable,*args], stdout=f, stderr=f)

def runWorker(*args):
    with open(os.devnull, "w") as f:
        return Popen([workerExecutable,*args], stdout=f, stderr=f)

def test1():
    inst = []
    inst.append(runServer())
    inst.append(runWorker())
    inst.append(runWorker())
    inst.append(runWorker())

    p = Popen([adminExecutable, 'localhost', '-l', '-d'],stdout=PIPE,bufsize=1)
    out = p.communicate()
    out = out[0].decode()
    # print(out)

    expectedOut='''
0 -> ::1
1 -> ::1
2 -> ::1
'''

    for i in inst:
        i.kill()

    lines1 = [l.strip() for l in out.strip().split('\n')         if l.strip()!='']
    lines2 = [l.strip() for l in expectedOut.strip().split('\n') if l.strip()!='']
    # print(lines1)
    # print(lines2)
    return lines1 == lines2

if __name__ == "__main__":
    print("running tests")
    tests = [test1]
    for i,t in enumerate(tests):
        if not t():
            print("test",i,"failed")
        else:
            print("test",i,"passed")
