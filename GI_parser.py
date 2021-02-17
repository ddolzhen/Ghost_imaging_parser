import numpy as np


class Frame:
    def __init__(self, frametime, frame_matrix):
        self.time = frametime
        self.data = frame_matrix


class FrameBase:

    def __init__(self, filename):

        self.frameList=[]
        frame_file = open(filename, 'r')
        lines = frame_file.readlines()

        frame = np.empty((0, 256), int)

        frame_ct = 0
        row_ct = 0
        for line in lines:
            if "### Frame" in line.strip():
                row_ct = 0
                frametime = line.split("at ")[1]
                if frame.size != 0:
                    frame_obj = Frame(frametime, frame)
                    self.frameList.append(frame_obj)
                    frame = np.empty((0, 256), int)
                    frametime=""
                    frame_ct += 1

            else:
                str_list = line.strip().split(',')[:-1]
                row = [int(i) for i in str_list]
                frame = np.vstack([frame, row])
                row_ct += 1


