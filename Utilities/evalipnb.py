import os
import sys
import time

from Queue import Empty

try:
    from IPython.kernel import KernelManager
except ImportError:
    from IPython.zmq.blockingkernelmanager import BlockingKernelManager as KernelManager

from IPython.nbformat.current import reads, NotebookNode

def run_notebook(nb):
    km = KernelManager()
    km.start_kernel(stderr=open(os.devnull, 'w'))
    try:
        kc = km.client()
    except AttributeError:
        # 0.13
        kc = km
    kc.start_channels()
    shell = kc.shell_channel
    # simple ping:
    shell.execute("pass")
    reply = shell.get_msg()
    
    cells = 0
    failures = 0
    for ws in nb.worksheets:
        for cell in ws.cells:
            if cell.cell_type != 'code':
                continue
            shell.execute(cell.input)
            # wait for finish, maximum 20s
            reply = shell.get_msg(timeout=20)['content']
            if reply['status'] == 'error':
                failures += 1
                print "\nFAILURE:"
                print cell.input
                print '-----'
                print "raised:"
                print '\n'.join(reply['traceback'])
            cells += 1
            sys.stdout.write('.')

    print
    print "ran notebook %s" % nb.metadata.name
    print "    ran %3i cells" % cells
    if failures:
        print "    %3i cells raised exceptions" % failures
    kc.stop_channels()
    km.shutdown_kernel()
    del km
    
    if failures:
        return 1
    return 0
    
    
if __name__ == '__main__':
    # Open the IPython notebook
    with open(sys.argv[1]) as f:
        nb = reads(f.read(), 'json')
    
    # This is typically set in cmake
    os.environ['TubeTK_BINARY_DIR'] = sys.argv[2]
    sys.exit(run_notebook(nb))
   
        
        