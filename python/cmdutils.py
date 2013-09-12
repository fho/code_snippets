import logging
import subprocess
import os

log = logging.getLogger(__name__)


def exec_cmd(cmd, working_dir="", successReturnValue=0, env=None):
    oldDir = os.getcwd()
    if(working_dir):
        os.chdir(working_dir)
    cmd = [str(i) for i in cmd]

    log.debug("Executing: %s" % cmd)
    try:
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE,
                             stderr=subprocess.STDOUT, env=env)
    except Exception, e:
        error_str = "Error executing %s: %s" % (cmd, e)
        log.error(error_str)
        raise(e)
    finally:
        os.chdir(oldDir)

    stdout = ""
    for line in iter(p.stdout.readline, ''):
        log.debug(line.rstrip())
        stdout += line

    if p.wait() != successReturnValue:
        error_str = ("Unexpected return value, command: %s, rtval: %s,"
                     "command output: \"%s\"" % (cmd, p.returncode, stdout))
        log.error(error_str)
        raise subprocess.CalledProcessError(p.returncode, cmd)

    return stdout
