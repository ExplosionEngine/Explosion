import os
import re
import platform
import subprocess
from sys import argv
from pathlib import Path

def get_nodejs_version(py_path):
    third_party_cmake = Path(py_path).parent.parent / 'ThirdParty' / 'CMakeLists.txt'
    if not third_party_cmake.exists():
        raise RuntimeError('Third-party CMakeLists.txt not found')

    nodejs_version_line = None
    with open(third_party_cmake) as cmake_file:
        for line in cmake_file.readlines():
            if 'NODEJS_VERSION' in line:
                nodejs_version_line = line
                break

    if nodejs_version_line is None:
        raise RuntimeError('NODEJS_VERSION define in third_party CMakeLists.txt not found')

    found = re.findall(r'.*NODEJS_VERSION \"(.+?)\".*', nodejs_version_line)
    return found[0]


def get_nodejs_root(py_path, nodejs_version):
    nodejs_root = Path(py_path).parent.parent / 'ThirdParty' / 'Lib' / '{}-{}-{}'.format('Nodejs', platform.system(), nodejs_version)
    if not nodejs_root.exists():
        raise RuntimeError('Root of node.js not found')
    return nodejs_root


def get_web_project_root(py_path):
    web_project_root = Path(py_path).parent.parent / 'Editor' / 'Web'
    return web_project_root


def start_dev_server(nodejs_root, web_project_root):
    env = os.environ.copy()
    if platform.system() == 'Windows':
        env['PATH'] = '{};{}'.format(env['PATH'], os.path.normpath(nodejs_root))
        subprocess.run(['npm', 'run', 'dev'], shell=True, encoding='utf-8', env=env, cwd=web_project_root)
    elif platform.system() == 'Darwin':
        nodejs_bin = Path(nodejs_root) / 'bin'
        env['PATH'] = '{}:{}'.format(env['PATH'], nodejs_bin)
        subprocess.run(['npm', 'run', 'dev'], encoding='utf-8', env=env, cwd=web_project_root)
    else:
        raise RuntimeError('Unsupported OS')


if __name__ == '__main__':
    py_path = os.path.realpath(__file__)
    print('Python File Path:', py_path)
    nodejs_version = get_nodejs_version(py_path)
    print('Node.js version:', nodejs_version)
    nodejs_root = get_nodejs_root(py_path, nodejs_version)
    print('Node.js root:', nodejs_root)
    web_project_root = get_web_project_root(py_path)
    print('Web project root:', web_project_root)
    start_dev_server(nodejs_root, web_project_root)
