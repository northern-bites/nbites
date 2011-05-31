
# import sys

# try:
#     import naovision
# except ImportError, e:
#     try:
#         import aibovision
#     except ImportError, e2:
#         print >>sys.stderr, "Error importing C++ Vision extension"
#         print >>sys.stderr, "For Nao:", e
#         print >>sys.stderr, "For Aibo:", e2
#     else:
#         vision_instance = aibovision.aibo
# else:
#     vision_instance = naovision.nao

# def Vision():
#     return vision_instance
