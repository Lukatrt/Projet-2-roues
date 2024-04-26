package main

import (
	"log"
	"net/http"
)

func main() {
	// Définir le dossier contenant les fichiers statiques
	fs := http.FileServer(http.Dir("/app")) // Modification ici

	// Gérer toutes les requêtes par le gestionnaire de fichiers
	http.Handle("/", fs)

	// Définir le port d'écoute du serveur
	log.Println("Le serveur écoute sur le port : 9999")
	err := http.ListenAndServe(":9999", nil)
	if err != nil {
		log.Fatal(err)
	}
}
