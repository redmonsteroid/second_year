// Храним исходные книги
let booksData = [];

// activeFilters: несколько Set-ов, по категориям
let activeFilters = {
  authors: new Set(),
  teacher: new Set(),
  publisher: new Set(),
  publication_city: new Set(),
  publication_year: new Set(),
  title: new Set()
};

// Списки уникальных значений
let uniqueValuesByField = {
  authors: [],
  teacher: [],
  publisher: [],
  publication_city: [],
  publication_year: [],
  title: []
};

document.addEventListener("DOMContentLoaded", () => {
  loadBooks();

  // Кнопка "Filters"
  const filtersToggleBtn = document.getElementById("filters-toggle-btn");
  if (filtersToggleBtn) {
    filtersToggleBtn.addEventListener("click", toggleFiltersPanel);
  }

  // Кнопка "Close" внутри панели
  const closeFiltersBtn = document.getElementById("close-filters-btn");
  if (closeFiltersBtn) {
    closeFiltersBtn.addEventListener("click", closeFiltersPanel);
  }

  // Кнопка "Clear All Filters"
  const clearBtn = document.getElementById("clear-filters-btn");
  if (clearBtn) {
    clearBtn.addEventListener("click", clearAllFilters);
  }

  // Закрыть панель при клике вне
  document.addEventListener("click", (e) => {
    const panel = document.getElementById("filters-panel");
    const btn = document.getElementById("filters-toggle-btn");
    if (!panel.contains(e.target) && e.target !== btn && !btn.contains(e.target)) {
      // Если панель открыта и клик был вне неё и вне кнопки, закрываем
      if (panel.classList.contains("show")) {
        panel.classList.remove("show");
      }
    }
  });

  // Предотвращаем распространение кликов внутри панели фильтров
  const panel = document.getElementById("filters-panel");
  if (panel) {
    panel.addEventListener("click", (e) => {
      e.stopPropagation();
    });
  }

  // Предотвращаем распространение кликов внутри каждого подменю
  const subMenus = document.querySelectorAll(".sub-menu");
  subMenus.forEach(subMenu => {
    subMenu.addEventListener("click", (e) => {
      e.stopPropagation();
    });
  });
});

/* ------------------ Загрузка книг с сервера ------------------ */
async function loadBooks() {
  const token = localStorage.getItem("token");
  if (!token) {
    alert("Unauthorized. Please login.");
    window.location.href = "login.html";
    return;
  }

  try {
    const response = await fetch("http://127.0.0.1:8000/books/", {
      headers: { Authorization: `Bearer ${token}` }
    });
    if (!response.ok) {
      throw new Error("Failed to load books");
    }

    booksData = await response.json();
    buildUniqueValues(booksData);
    renderAllSubMenus();

    displayBooks(booksData);
  } catch (error) {
    console.error(error);
    alert("Error loading books");
  }
}

/* ------------------ Сбор уникальных значений ------------------ */
function buildUniqueValues(books) {
  for (let field in uniqueValuesByField) {
    uniqueValuesByField[field] = [];
  }

  const setAuthors = new Set();
  const setTeacher = new Set();
  const setPublisher = new Set();
  const setCity = new Set();
  const setYear = new Set();
  const setTitle = new Set();

  books.forEach(book => {
    if (book.authors) {
      book.authors.forEach(a => {
        const fn = `${a.last_name} ${a.first_name}${a.middle_name ? " " + a.middle_name : ""}`.trim();
        setAuthors.add(fn);
      });
    }
    if (book.owner_username) setTeacher.add(book.owner_username);
    if (book.publisher_rel && book.publisher_rel.name) setPublisher.add(book.publisher_rel.name);
    if (book.publisher_rel && book.publisher_rel.city) setCity.add(book.publisher_rel.city);
    if (book.publication_year) setYear.add(String(book.publication_year));
    if (book.title) setTitle.add(book.title);
  });

  uniqueValuesByField.authors = Array.from(setAuthors).sort();
  uniqueValuesByField.teacher = Array.from(setTeacher).sort();
  uniqueValuesByField.publisher = Array.from(setPublisher).sort();
  uniqueValuesByField.publication_city = Array.from(setCity).sort();
  uniqueValuesByField.publication_year = Array.from(setYear).sort();
  uniqueValuesByField.title = Array.from(setTitle).sort();
}

/* ------------------ Генерация чекбоксов ------------------ */
function renderAllSubMenus() {
  for (let field in uniqueValuesByField) {
    const arr = uniqueValuesByField[field];
    const subMenuListId = "sub-" + field + "-list";
    const subMenuListElem = document.getElementById(subMenuListId);
    if (!subMenuListElem) continue;

    subMenuListElem.innerHTML = "";
    arr.forEach(val => {
      const li = document.createElement("li");
      const label = document.createElement("label");
      label.classList.add("checkbox-label");

      const cb = document.createElement("input");
      cb.type = "checkbox";
      cb.value = val;
      cb.addEventListener("change", () => onFilterCheck(field, cb));

      label.appendChild(cb);
      label.appendChild(document.createTextNode(val));
      li.appendChild(label);
      subMenuListElem.appendChild(li);
    });
  }
}

/* ------------------ Обработка чекбоксов ------------------ */
function onFilterCheck(category, checkboxElem) {
  const val = checkboxElem.value;
  if (checkboxElem.checked) {
    activeFilters[category].add(val);
  } else {
    activeFilters[category].delete(val);
  }
  applyFilters();
}

/* ------------------ Фильтр подменю (поиском) ------------------ */
function filterSubMenu(category, searchValue) {
  searchValue = searchValue.trim().toLowerCase();
  const subMenuListId = "sub-" + category + "-list";
  const subMenuListElem = document.getElementById(subMenuListId);
  if (!subMenuListElem) return;

  const items = subMenuListElem.querySelectorAll("li");
  items.forEach(li => {
    const text = li.textContent.toLowerCase();
    if (text.includes(searchValue)) {
      li.style.display = "";
    } else {
      li.style.display = "none";
    }
  });
}

/* ------------------ Применение фильтров ------------------ */
function applyFilters() {
  // Проверяем, есть ли что-то в activeFilters
  const allEmpty = Object.values(activeFilters).every(s => s.size === 0);
  let filtered = [];

  if (allEmpty) {
    filtered = [...booksData];
  } else {
    filtered = booksData.filter(book => {
      // authors
      if (activeFilters.authors.size > 0) {
        if (!book.authors) return false;
        const names = book.authors.map(a =>
          `${a.last_name} ${a.first_name}${a.middle_name ? " " + a.middle_name : ""}`.trim()
        );
        const hasAny = names.some(name => activeFilters.authors.has(name));
        if (!hasAny) return false;
      }
      // teacher
      if (activeFilters.teacher.size > 0) {
        if (!activeFilters.teacher.has(book.owner_username)) return false;
      }
      // publisher
      if (activeFilters.publisher.size > 0) {
        const pub = book.publisher_rel ? book.publisher_rel.name : "";
        if (!activeFilters.publisher.has(pub)) return false;
      }
      // city
      if (activeFilters.publication_city.size > 0) {
        const c = book.publisher_rel ? book.publisher_rel.city : "";
        if (!activeFilters.publication_city.has(c)) return false;
      }
      // year
      if (activeFilters.publication_year.size > 0) {
        const y = String(book.publication_year || "");
        if (!activeFilters.publication_year.has(y)) return false;
      }
      // title
      if (activeFilters.title.size > 0) {
        if (!activeFilters.title.has(book.title)) return false;
      }
      return true;
    });
  }

  displayBooks(filtered);
  renderSelectedFilters();
}

/* ------------------ Отображение книг ------------------ */
function displayBooks(books) {
  const bookList = document.getElementById("book-list");
  if (!bookList) return;
  bookList.innerHTML = "";

  books.forEach(book => {
    const publisherName = sanitizeHTML(book.publisher_rel ? book.publisher_rel.name : "N/A");
    const publisherCity = sanitizeHTML(book.publisher_rel ? book.publisher_rel.city : "N/A");

    let authorsText = "N/A";
    if (book.authors && book.authors.length > 0) {
      authorsText = book.authors
        .map(a => sanitizeHTML(`${a.last_name} ${a.first_name}${a.middle_name ? " " + a.middle_name : ""}`))
        .join(", ");
    }

    const downloadLink = book.download_link ? sanitizeAttribute(book.download_link) : "N/A";

    // Сокращаем ссылку для отображения (например, первые 30 символов)
    const displayLink = downloadLink !== "N/A" && downloadLink.length > 30 
      ? `${downloadLink.substring(0, 30)}...` 
      : downloadLink;

    const li = document.createElement("li");
    li.innerHTML = `
      <strong>Title:</strong> ${sanitizeHTML(book.title) || "N/A"} <br>
      <strong>Authors:</strong> ${authorsText} <br>
      <strong>Publisher:</strong> ${publisherName} <br>
      <strong>City:</strong> ${publisherCity} <br>
      <strong>Year:</strong> ${book.publication_year ?? "N/A"} <br>
      <strong>Pages:</strong> ${book.page_count ?? "N/A"} <br>
      <strong>Additional Info:</strong> ${sanitizeHTML(book.additional_info) || "N/A"} <br>
      <strong>Download Link:</strong> ${
        downloadLink !== "N/A"
          ? `<div class="link-container">
               <a href="${downloadLink}" target="_blank" title="${downloadLink}">${displayLink}</a>
               <button class="copy-btn" onclick="copyToClipboard('${sanitizeJS(downloadLink)}')"><i class="fas fa-copy"></i></button>
             </div>`
          : "N/A"
      } <br>
    `;
    bookList.appendChild(li);
  });
}

/* ------------------ Функции для санитизации данных ------------------ */
function sanitizeHTML(str) {
  if (!str) return "";
  return str.replace(/&/g, "&amp;")
            .replace(/</g, "&lt;")
            .replace(/>/g, "&gt;")
            .replace(/"/g, "&quot;")
            .replace(/'/g, "&#039;");
}

function sanitizeAttribute(str) {
  if (!str) return "";
  return str.replace(/"/g, "&quot;").replace(/'/g, "&#039;");
}

function sanitizeJS(str) {
  if (!str) return "";
  return str.replace(/'/g, "\\'");
}

/* ------------------ Чипы выбранных фильтров ------------------ */
function renderSelectedFilters() {
  const container = document.getElementById("selected-filters");
  if (!container) return;
  container.innerHTML = "";

  for (let field in activeFilters) {
    activeFilters[field].forEach(value => {
      const chip = document.createElement("span");
      chip.className = "filter-chip";
      chip.textContent = `${field}: ${value}`;
      chip.addEventListener("click", (e) => {
        e.stopPropagation(); // Предотвращаем распространение события
        removeFilter(field, value);
      });
      container.appendChild(chip);
    });
  }
}

/* Удалить конкретный фильтр */
function removeFilter(category, value) {
  activeFilters[category].delete(value);

  // Снимаем чекбокс
  const subMenuListId = "sub-" + category + "-list";
  const subMenuListElem = document.getElementById(subMenuListId);
  if (subMenuListElem) {
    const cbs = subMenuListElem.querySelectorAll("input[type='checkbox']");
    cbs.forEach(cb => {
      if (cb.value === value) {
        cb.checked = false;
      }
    });
  }
  applyFilters();
}

/* Очистить все фильтры */
function clearAllFilters() {
  for (let field in activeFilters) {
    activeFilters[field].clear();
  }
  const allCheckboxes = document.querySelectorAll(".sub-menu input[type='checkbox']");
  allCheckboxes.forEach(cb => {
    cb.checked = false;
  });

  displayBooks(booksData);
  renderSelectedFilters();
}

/* ------------------ Открыть/закрыть панель фильтров ------------------ */
function toggleFiltersPanel() {
  const panel = document.getElementById("filters-panel");
  panel.classList.toggle("show");
}

function closeFiltersPanel() {
  const panel = document.getElementById("filters-panel");
  panel.classList.remove("show");
}

/* ------------------ Подменю внутри категории ------------------ */
function toggleSubMenu(category) {
  const subMenu = document.getElementById("sub-" + category);
  if (!subMenu) return;
  // Если display:none -> показать, иначе скрыть
  const st = window.getComputedStyle(subMenu).display;
  subMenu.style.display = (st === "none" || st === "block") ? (st === "none" ? "block" : "none") : "block";
}

/* ------------------ Логаут ------------------ */
function logout() {
  localStorage.removeItem("token");
  localStorage.removeItem("role");
  window.location.href = "login.html";
}

/* ------------------ Функция для отображения Toast уведомлений ------------------ */
function showToast(msg, type="success") {
  const container = document.getElementById("toast-container");
  if (!container) return;
  const toast = document.createElement("div");
  toast.classList.add("toast");
  if (type === "error") {
    toast.style.backgroundColor = "#b52f2b";
  } else if (type === "success") {
    toast.style.backgroundColor = "#28a745";
  }
  toast.textContent = msg;

  container.appendChild(toast);

  // Удаляем уведомление через 3 секунды
  setTimeout(() => {
    if (toast.parentNode) {
      toast.parentNode.removeChild(toast);
    }
  }, 3000);
}

/* ------------------ Функция для копирования текста в буфер обмена ------------------ */
function copyToClipboard(text) {
  navigator.clipboard.writeText(text).then(() => {
    showToast("Ссылка скопирована в буфер обмена!", "success");
  }).catch(err => {
    console.error('Не удалось скопировать: ', err);
    showToast("Не удалось скопировать ссылку", "error");
  });
}
