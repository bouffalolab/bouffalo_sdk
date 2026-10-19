const documents = {
    en: {
        uri: "/docs/readme-en.md",
        title: "README.md",
        htmlLanguage: "en"
    },
    zh: {
        uri: "/docs/readme-zh.md",
        title: "README_zh.md",
        htmlLanguage: "zh-CN"
    }
};

const content = document.getElementById("readme-content");
const loadingState = document.getElementById("loading-state");
const errorState = document.getElementById("error-state");
const errorMessage = document.getElementById("error-message");
const documentSource = document.getElementById("document-source");
const documentSize = document.getElementById("document-size");
const documentTitle = document.getElementById("document-title");
const languageButtons = document.querySelectorAll(".language-button");

let activeLanguage = "en";

function formatBytes(byteCount) {
    if (byteCount < 1024) {
        return byteCount + " bytes";
    }
    return (byteCount / 1024).toFixed(1) + " KiB";
}

function setActiveLanguage(language) {
    activeLanguage = language;
    languageButtons.forEach(function (button) {
        const isActive = button.dataset.language === language;
        button.classList.toggle("active", isActive);
        button.setAttribute("aria-pressed", isActive ? "true" : "false");
    });
}

async function loadDocument(language) {
    const documentInfo = documents[language];

    if (!documentInfo) {
        return;
    }

    setActiveLanguage(language);
    document.documentElement.lang = documentInfo.htmlLanguage;
    documentSource.textContent = documentInfo.uri;
    documentTitle.textContent = documentInfo.title;
    documentSize.textContent = "Loading…";
    content.hidden = true;
    errorState.hidden = true;
    loadingState.hidden = false;

    try {
        const response = await fetch(documentInfo.uri, { cache: "no-store" });
        if (!response.ok) {
            throw new Error("HTTP " + response.status + " " + response.statusText);
        }

        const markdown = await response.text();
        content.textContent = markdown;
        content.scrollTop = 0;
        documentSize.textContent = formatBytes(new TextEncoder().encode(markdown).length);
        loadingState.hidden = true;
        content.hidden = false;
    } catch (error) {
        loadingState.hidden = true;
        errorMessage.textContent = error.message || "The device did not return a valid response.";
        errorState.hidden = false;
    }
}

languageButtons.forEach(function (button) {
    button.addEventListener("click", function () {
        loadDocument(button.dataset.language);
    });
});

document.getElementById("reload-button").addEventListener("click", function () {
    loadDocument(activeLanguage);
});

document.getElementById("retry-button").addEventListener("click", function () {
    loadDocument(activeLanguage);
});

loadDocument("en");
