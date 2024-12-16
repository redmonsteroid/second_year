FROM tiangolo/uwsgi-nginx-flask:python3.12


COPY ./crypt /app

ENV PYTHONUNBUFFERED 1

EXPOSE 5000
WORKDIR /app

CMD ["python", "app2.py"]

