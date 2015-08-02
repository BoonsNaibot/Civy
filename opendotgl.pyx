cdef WidgetMetaclass(type):
    cdef GLuint vao
    
    def __init__(mcs, name bases, attrs):
        #glGenVertexArrays(1, self.vao)
        
        cdef str canvas = attrs.pop("cv")
        self.create_canvas(canvas)


cdef Widget(object):
    cdef GLuint vbo
    
    def __cinit__(self, *args):
        cdef object vao = self.__metaclass__
        cdef GLuint cvao = vao.get_vao()

        glBindVertexArray(vao)
        glGenBuffers(1, self.vbo)
        
        
    def __draw__(self, *args):
        
