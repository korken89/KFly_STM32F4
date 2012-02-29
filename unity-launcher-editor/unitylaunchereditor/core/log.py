# -*- coding: utf-8 -*-
# Copyright (c) 2010 Rafa Muñoz Cárdenas
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#   * Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#   * Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#   * Neither the name of Pioneers of the Inevitable, Songbird, nor the names
#     of its contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import logging
from unitylaunchereditor.core.translation import APP

class Singleton(object):
    """
    Singleton interface:
    http://www.python.org/download/releases/2.2.3/descrintro/#__new__
    """
    def __new__(cls, *args, **kwds):
        it = cls.__dict__.get("__it__")
        if it is not None:
            return it
        cls.__it__ = it = object.__new__(cls)
        it.init(*args, **kwds)
        return it

    def init(self, *args, **kwds):
        pass

class LoggerManager(Singleton):
    """
    Logger Manager.
    Handles all logging files.
    """
    def init(self):
        self.logger = logging.getLogger()
        
        handler = None
        handler = logging.StreamHandler()# logging.FileHandler(LOG_FILENAME, "a")
        self.logger.setLevel(logging.DEBUG)
        formatter = logging.Formatter("%(asctime)s::%(levelname)s::%(name)s -> %(message)s")
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)

    def debug(self, loggername, msg):
        self.logger = logging.getLogger(loggername)
        self.logger.debug(msg)

    def error(self, loggername, msg):
        self.logger = logging.getLogger(loggername)
        self.logger.error(msg)

    def info(self, loggername, msg):
        self.logger = logging.getLogger(loggername)
        self.logger.info(msg)

    def warning(self, loggername, msg):
        self.logger = logging.getLogger(loggername)
        self.logger.warning(msg)

class Logger(object):
    """
    Logger object.
    """
    def __init__(self, loggername=APP):
        self.lm = LoggerManager() # LoggerManager instance
        self.loggername = loggername # logger name

    def debug(self, msg):
        self.lm.debug(self.loggername, msg)

    def error(self, msg):
        self.lm.error(self.loggername, msg)

    def info(self, msg):
        self.lm.info(self.loggername, msg)

    def warning(self, msg):
        self.lm.warning(self.loggername, msg)
if __name__ == "__main__":
    x = Logger(__file__)
    x.warning('msg')
    