http://adler3d.github.io/qap_vm/trash/test2017/hello.html

TODO:
Удалить кнопку fullscreen
Растянуть canvas на весь документ
Рисовать лого emscripten при загрузке в центре сanvas`а
В случае ошибки делать что-то типа: document.body.innerHTML="<pre>"+errmsg;

Подумать только:
Залить папку с emscripten в zip/rar-архиве сюда на гитхаб.
Залить исходники своего проекта на emscripten сюда на гитхаб.
Залить бинарники своего компилятор машкода сюда на гитхаб.

Что должно быть в итоге:
У юзера должна быть возможность в один клик выложить свой проект сюда.
Тут по началу должна быть ссылка на хранилище всех выложеных кем-либо скриптов.
Тут должна быть ссылка на хранилище всех(активных||принятых||оценённых||новых) скриптов.

Хранилище скриптов{}или{
  nodejs{
    Написать скрипт который будет хранить скрипты у себя в памяти.
        
    on_request_put_file=(POST)=>{g_files[fn]=content;}
    on_request_list=()=>Object.keys(g_files).join("\n");
    on_request_get_file=()=>g_files[fn];
    
    у файла должно быть (короткое_не_уникальное_имя&&длинное_уникальное_имя&&какое_то_описание&&preview_img)
    
    в конечном итоге preview-картинка должна выбираться автоматически.
    
    тест готов:
    http://vm-vm.44fs.preview.openshiftapps.com/put?fn=our_file.txt&data=some_content
    
    host:
    https://console.preview.openshift.com/console/project/vm/overview
  }
  php{
    Написать скрипт который будет хранить скрипты в файловой системе, а список файлов - в спецальном файле.
  }
}

Hosts:{
  forever: http://vm-vm.44fs.preview.openshiftapps.com/
  30days(from 2017.02.19): http://vm-96205.app.xervo.io
}
