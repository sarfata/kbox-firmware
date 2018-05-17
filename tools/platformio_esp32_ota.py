Import('env')

env.Replace(UPLOADOTACMD='curl -F "image=@$SOURCE" http://$UPLOAD_PORT/update')
