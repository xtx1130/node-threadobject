{'targets': [
      {
           'target_name': 'node-threadobject',
           'win_delay_load_hook': 'false',
           'conditions':[
                 [
                    'OS=="win"', {
                       'sources': [
                            'src/rcib.cc',
                            'src/rcib_object.cc',
                            'src/globals.cc',
                            'src/rcib/at_exist.cc',
                            'src/rcib/lazy_instance.cc',
                            'src/rcib/MessageLoop.cc',
                            'src/rcib/MessagePumpDefault.cc',
                            'src/rcib/PendingTask.cc',
                            'src/rcib/ref_counted.cc',
                            'src/rcib/Thread.cc',
                            'src/rcib/util_tools.cc',
                            'src/rcib/WeakPtr.cc',
                            'src/rcib/Event/WaitableEvent.cc',
                            'src/rcib/time/time.cc'
                            ,'src/rcib/roler.cc'
                            ,'src/hash/hash.cc'
                            ,'src/hash/sha-256/sha-256.cc'
                            ,'src/file/file.cc'
                            ,'src/delayed/delayed.cc'
                            ,'src/rcib.h'
                            ,'src/rcib_object.h'
                            ,'src/hash/hash.h'
                            ,'src/hash/sha-256/sha-256.h'
                            ,'src/file/file.h'
                            ,'src/delayed/delayed.h'
                        ],
                        'libraries':[],
                        'cflags':[]
                    }],[
                        'OS=="linux"', {
                             'sources': [
                             'src/rcib.cc',
                             'src/rcib_object.cc',
                             'src/globals.cc',
                             'src/rcib/at_exist.cc',
                             'src/rcib/lazy_instance.cc',
                             'src/rcib/MessageLoop.cc',
                             'src/rcib/MessagePumpDefault.cc',
                             'src/rcib/PendingTask.cc',
                             'src/rcib/ref_counted.cc',
                             'src/rcib/Thread.cc',
                             'src/rcib/util_tools.cc',
                             'src/rcib/WeakPtr.cc',
                             'src/rcib/Event/WaitableEvent.cc',
                             'src/rcib/time/time.cc'
                             ,'src/rcib/roler.cc'
                             ,'src/hash/hash.cc'
                             ,'src/hash/sha-256/sha-256.cc'
                             ,'src/file/file.cc'
                             ,'src/delayed/delayed.cc'
                             ],
                             'libraries':[],
                             'cflags':[]
                        }]
                        ]}]}