# Utiliser l'image officielle de Go pour la phase de construction
FROM golang:1.18-alpine as builder

# Définir le répertoire de travail dans le conteneur de construction
WORKDIR /build

# Copier les fichiers de configuration Go
COPY go.mod .

# Télécharger les dépendances si go.mod et go.sum existent
RUN go mod download

# Copier le fichier source Go dans notre conteneur
COPY main.go .

# Compiler l'application Go
RUN go build -o main .

# Utiliser l'image Alpine pour la phase d'exécution
FROM alpine:latest

# Définir le répertoire de travail pour notre conteneur d'exécution
WORKDIR /app

# Installer ca-certificates, nécessaire pour les appels HTTPS sortants dans le cas où votre application en aurait besoin
RUN apk add --no-cache ca-certificates

# Copier l'exécutable compilé depuis la phase de construction
COPY --from=builder /build/main .

# Copier les fichiers statiques du site web dans le dossier /app
COPY *.html ./
COPY *.css ./
COPY *.jpeg ./
COPY *.png ./

# Exposer le port sur lequel le serveur sera accessible
EXPOSE 9999

# Commande pour exécuter l'application
CMD ["./main"]
