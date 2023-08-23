python -m pip install glad
python -m glad --generator=c --spec gl --out-path=GL
python -m glad --generator=c --spec wgl --out-path=GL
python -m glad --generator=c --spec glx --out-path=GL