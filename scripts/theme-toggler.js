// Costanti DOM
const htmlElement = document.documentElement;
const switchBtn = document.getElementById("themeToggle");
const themeDiv = document.getElementById("themeDiv");
let lblTema, iconaTema;

// Funzione per ottenere il tema iniziale
function getInitialTheme() {
    const temaSalvato = localStorage.getItem("bsTheme");
    const preferisceScuro = window.matchMedia('(prefers-color-scheme: dark)').matches;
    let uscita;
    
    if (temaSalvato) {
        uscita = temaSalvato;
    } else if (preferisceScuro) {
        uscita = "dark";
    } else {
        uscita = "light";
    }

    return uscita;
}

// Funzione per applicare il tema
function applyTheme(theme) {
    htmlElement.setAttribute('data-bs-theme', theme);
    if (theme === "dark") {
        switchBtn.checked = true;
    }
}

// Funzione per creare/aggiornare la label del tema
function updateThemeLabel(theme) {
    // Rimuovi la label esistente se presente
    const lblTemaEsistente = themeDiv.querySelector(".form-check-label");
    if (lblTemaEsistente) {
        lblTemaEsistente.remove();
    }
    
    // Crea nuova label
    lblTema = document.createElement("label");
    lblTema.setAttribute("class", "form-check-label nav-link");
    lblTema.setAttribute("for", "themeToggle");
    
    if (theme === "dark") {
        lblTema.textContent = "Dark Theme";
    } else {
        lblTema.textContent = "Light Theme";
    }
    
    themeDiv.append(lblTema);
}

// Funzione per creare/aggiornare l'icona del tema
function updateThemeIcon(theme) {
    // Rimuovi l'icona esistente se presente
    const iconaTemaEsistente = themeDiv.querySelector(".bi");
    if (iconaTemaEsistente) {
        iconaTemaEsistente.remove();
    }
    
    // Crea nuova icona
    iconaTema = document.createElement("span");
    
    if (theme === "dark") {
        // Imposto la classe dell'icona allo span
        iconaTema.setAttribute("class", "bi bi-moon");
    } else {
        iconaTema.setAttribute("class", "bi bi-sun");
    }
    
    themeDiv.append(iconaTema);
}

// Funzione per gestire il cambio tema
function handleThemeSwitch() {
    let nuovoTema;
    
    if (switchBtn.checked) {
        nuovoTema = "dark";
    } else {
        nuovoTema = "light";
    }

    updateThemeLabel(nuovoTema);
    updateThemeIcon(nuovoTema);
    htmlElement.setAttribute("data-bs-theme", nuovoTema);
    localStorage.setItem("bsTheme", nuovoTema);
}

// Funzione per abilitare i tooltip di BootStrap
function initTooltips() {
    let elementi = document.querySelectorAll("[data-bs-toggle='tooltip']");
    let i;

    for (i = 0; i < elementi.length; i++) {
        new bootstrap.Tooltip(elementi[i]);
    }
}

// Funzione di inizializzazione
function init() {
    const temaCorrente = getInitialTheme();
    applyTheme(temaCorrente);
    updateThemeLabel(temaCorrente);
    updateThemeIcon(temaCorrente);
    initTooltips();
}

// Event listener per il tasto
switchBtn.addEventListener("change", handleThemeSwitch);
// Event listener per il caricamento del DOM
document.addEventListener("DOMContentLoaded", init);