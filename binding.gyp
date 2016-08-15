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
                             ],
                             'libraries':[],
                             'cflags':[]
                        }]
                        ]}]}