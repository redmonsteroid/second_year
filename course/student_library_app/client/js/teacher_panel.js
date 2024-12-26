// Глобальные переменные
let editingBookId = null;  // Идентификатор книги в режиме редактирования
let teacherBooks = [];     // Все книги учителя
let filteredBooks = [];    // Отфильтрованные книги (при поиске)

// При загрузке документа выполняем начальную настройку
document.addEventListener("DOMContentLoaded", () => {
  const token = localStorage.getItem("token");
  const role = localStorage.getItem("role");
  if (!token || role !== "teacher") {
    alert("Access denied. You are not a teacher.");
    window.location.href = "../pages/library.html";
    return;
  }

  loadBooks();

  // Обработчик отправки формы добавления/редактирования книги
  const bookForm = document.getElementById("book-form");
  if (bookForm) {
    bookForm.addEventListener("submit", handleBookFormSubmit);
  }

  // Обработчик клика вне модального окна для его закрытия
  const modal = document.getElementById("book-modal");
  if (modal) {
    modal.addEventListener("click", (e) => {
      if (e.target === modal) {
        hideModal();
      }
    });
  }

  // Обработчик переключения темы (предполагается, что функция toggleTheme определена в main.js)
  const themeToggleBtn = document.getElementById("theme-toggle-menu");
  if (themeToggleBtn) {
    themeToggleBtn.addEventListener("click", toggleTheme);
  }

  // Обработчик открытия/закрытия бокового меню (предполагается, что функции toggleSidebar определены в main.js)
  const menuToggleBtn = document.getElementById("menu-toggle");
  if (menuToggleBtn) {
    menuToggleBtn.addEventListener("click", toggleSidebar);
  }
});

// Функции валидации
function validateRequired(value, fieldName) {
  const trimmed = value.trim();
  if (!trimmed) {
    return `Please provide a valid ${fieldName}. This field cannot be empty or only spaces.`;
  }
  return "";
}

function validatePublicationYear(value) {
  const trimmed = value.trim();
  if (!trimmed) {
    return { year: null, error: "" };
  }
  const parsed = parseInt(trimmed, 10);
  if (isNaN(parsed)) {
    return { year: null, error: "Publication year must be a valid number (4 digits)." };
  }
  if (trimmed.length !== 4) {
    return { year: null, error: "Publication year must be a 4-digit number." };
  }
  if (parsed < 0 || parsed > 2025) {
    return { year: null, error: "Publication year must be between 0 and 2025." };
  }
  return { year: parsed, error: "" };
}

function validatePageCount(value) {
  if (!value.trim()) {
    return { pageCount: null, error: "" }; // Необязательное поле
  }
  const parsed = parseInt(value, 10);
  if (isNaN(parsed)) {
    return { pageCount: null, error: "Page count must be a valid number." };
  }
  if (parsed < 1) {
    return { pageCount: null, error: "Page count must be a positive number." };
  }
  return { pageCount: parsed, error: "" };
}

// Функция загрузки книг с сервера
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
    const allBooks = await response.json();
    const currentUsername = localStorage.getItem("username");
    teacherBooks = allBooks.filter(book => book.owner_username === currentUsername);

    // По умолчанию отображаем все книги
    filteredBooks = [...teacherBooks];
    renderBooks(filteredBooks);
  } catch (error) {
    console.error(error);
    showToast("Error loading books", "error");
  }
}

// Функция отображения книг в карточках
function renderBooks(booksArr) {
  const bookCards = document.getElementById("book-cards");
  if (!bookCards) return;
  bookCards.innerHTML = "";

  booksArr.forEach(book => {
    const publisherName = book.publisher_rel ? book.publisher_rel.name : "N/A";
    const publisherCity = book.publisher_rel ? book.publisher_rel.city : "N/A";

    let authorsText = "N/A";
    if (book.authors && book.authors.length > 0) {
      authorsText = book.authors
        .map(a => `${sanitizeHTML(a.last_name)} ${sanitizeHTML(a.first_name)}${a.middle_name ? " " + sanitizeHTML(a.middle_name) : ""}`)
        .join(", ");
    }

    const card = document.createElement("div");
    card.classList.add("book-card");
    card.innerHTML = `
      <h4>${sanitizeHTML(book.title) || "N/A"}</h4>
      <p><strong>Authors:</strong> ${authorsText}</p>
      <p><strong>Publisher:</strong> ${sanitizeHTML(publisherName)}, <strong>City:</strong> ${sanitizeHTML(publisherCity)}</p>
      <p><strong>Year:</strong> ${book.publication_year ?? "N/A"}</p>
      <p><strong>Pages:</strong> ${book.page_count ?? "N/A"}</p>
      <p><strong>Link:</strong> ${
        book.download_link
          ? `<div class="link-container">
               <a href="${sanitizeAttribute(book.download_link)}" target="_blank" title="${sanitizeAttribute(book.download_link)}">${sanitizeText(book.download_link)}</a>
               <button class="copy-btn" onclick="copyToClipboard('${sanitizeJS(book.download_link)}')"><i class="fas fa-copy"></i></button>
             </div>`
          : "N/A"
      }</p>
      <p><strong>Info:</strong> ${sanitizeHTML(book.additional_info) || "N/A"}</p>
      <div class="book-actions">
        <button class="edit-btn" onclick="editBook(${book.id})">Edit</button>
        <button class="delete-btn" onclick="deleteBook(${book.id})">Delete</button>
      </div>
    `;
    bookCards.appendChild(card);
  });
}

// Функции для санитизации данных
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

function sanitizeText(str) {
  if (!str) return "";
  // Ограничим количество символов для отображения
  const maxLength = 50;
  if (str.length > maxLength) {
    return str.substring(0, maxLength) + '...';
  }
  return str;
}

function sanitizeJS(str) {
  if (!str) return "";
  return str.replace(/'/g, "\\'");
}

// Функция поиска книг учителя
function searchTeacherBooks(searchVal) {
  const val = searchVal.trim().toLowerCase();
  filteredBooks = teacherBooks.filter(b => {
    // Проверяем title, authors, publisher, city и т.д.
    const title = b.title ? b.title.toLowerCase() : "";
    const authors = b.authors
      ? b.authors.map(a => (a.last_name + " " + a.first_name + (a.middle_name || "")).toLowerCase()).join(" ")
      : "";
    const publisher = (b.publisher_rel && b.publisher_rel.name) ? b.publisher_rel.name.toLowerCase() : "";
    const city = (b.publisher_rel && b.publisher_rel.city) ? b.publisher_rel.city.toLowerCase() : "";

    return (
      title.includes(val) ||
      authors.includes(val) ||
      publisher.includes(val) ||
      city.includes(val)
    );
  });
  renderBooks(filteredBooks);
}

// Функции для открытия и закрытия модального окна
function openBookModal() {
  editingBookId = null;
  resetBookForm();
  document.getElementById("modal-title").textContent = "Add New Book";
  showModal();
}

function closeBookModal() {
  hideModal();
}

// Функция очистки формы и добавления поля для автора
function resetBookForm() {
  document.getElementById("editing-book-id").value = "";
  document.getElementById("book-title").value = "";
  document.getElementById("book-download-link").value = "";
  document.getElementById("book-publication-city").value = "";
  document.getElementById("book-publisher").value = "";
  document.getElementById("book-publication-year").value = "";
  document.getElementById("book-page-count").value = "";
  document.getElementById("book-additional-info").value = "";

  // Очищаем контейнер авторов и добавляем одну пустую запись
  const container = document.getElementById("authors-container");
  container.innerHTML = "";
  addAuthorField();

  clearAllErrors();
}

// Обработчик отправки формы (добавление/редактирование книги)
async function handleBookFormSubmit(e) {
  e.preventDefault();
  clearAllErrors();

  const token = localStorage.getItem("token");
  if (!token) {
    showToast("Unauthorized", "error");
    return;
  }

  // Получаем значения из формы
  const titleVal = document.getElementById("book-title").value;
  const downloadLinkVal = document.getElementById("book-download-link").value;
  const publicationCityVal = document.getElementById("book-publication-city").value;
  const publisherVal = document.getElementById("book-publisher").value;
  const yearVal = document.getElementById("book-publication-year").value;
  const pageCountVal = document.getElementById("book-page-count").value;
  const additionalInfoVal = document.getElementById("book-additional-info").value;
  const authors = getAuthors();

  // Валидация полей
  let hasError = false;

  // Валидация заголовка книги
  const titleError = validateRequired(titleVal, "Book Title");
  if (titleError) {
    showError("book-title-error", titleError);
    hasError = true;
  }

  // Валидация года публикации
  const { year, error: yearError } = validatePublicationYear(yearVal);
  if (yearError) {
    showError("book-publication-year-error", yearError);
    hasError = true;
  }

  // Валидация количества страниц
  const { pageCount, error: pageCountError } = validatePageCount(pageCountVal);
  if (pageCountError) {
    showError("book-page-count-error", pageCountError);
    hasError = true;
  }

  

  // Валидация издателя (можно добавить дополнительные проверки)
  // Например, если издательство обязательно, раскомментируйте следующий код
  /*
  if (!publisherVal.trim()) {
    showError("book-publisher-error", "Publisher cannot be empty.");
    hasError = true;
  }
  */

  if (hasError) {
    console.log("Validation failed.");
    return;
  }

  // Формируем объект publisher
  let publisherObj = null;
  if (publisherVal.trim() || publicationCityVal.trim()) {
    publisherObj = {
      name: publisherVal.trim() || null,
      city: publicationCityVal.trim() || null
    };
  }

  // Сбор данных для добавления или редактирования книги
  const bookData = {
    title: titleVal.trim(),
    download_link: downloadLinkVal.trim() || null,
    publisher: publisherObj, // Объект с полями name и city
    publication_year: year, // Уже валидирован
    page_count: pageCount, // Уже валидирован
    additional_info: additionalInfoVal.trim() || null,
    authors: authors
  };

  try {
    if (editingBookId) {
      // Режим редактирования: сначала удаляем старую книгу
      const deleteResponse = await fetch(`http://127.0.0.1:8000/books/${editingBookId}/`, {
        method: "DELETE",
        headers: { Authorization: `Bearer ${token}` },
      });

      if (!deleteResponse.ok) {
        throw new Error("Failed to delete the old book for editing");
      }

      // Затем добавляем новую книгу с обновлёнными данными
      const addResponse = await fetch("http://127.0.0.1:8000/books/", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: `Bearer ${token}`,
        },
        body: JSON.stringify(bookData),
      });

      if (!addResponse.ok) {
        const errorData = await addResponse.json();
        throw new Error(`Failed to add the updated book: ${JSON.stringify(errorData)}`);
      }

      showToast("Book updated successfully!", "success");
    } else {
      // Режим добавления
      const response = await fetch("http://127.0.0.1:8000/books/", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: `Bearer ${token}`,
        },
        body: JSON.stringify(bookData),
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(`Failed to add book: ${JSON.stringify(errorData)}`);
      }

      showToast("Book added successfully!", "success");
    }

    // Обновляем список книг и закрываем модальное окно
    await loadBooks();
    closeBookModal();
  } catch (error) {
    console.error(error);
    showToast(`Error ${editingBookId ? "updating" : "adding"} book: ${error.message}`, "error");
  }
}

// Функция для редактирования книги (заполнение формы и открытие модала)
function editBook(bookId) {
  const book = teacherBooks.find(b => b.id === bookId);
  if (!book) {
    showToast("Book not found", "error");
    return;
  }

  editingBookId = bookId;
  document.getElementById("modal-title").textContent = "Edit Book";

  // Заполняем поля формы
  document.getElementById("book-title").value = book.title || "";
  document.getElementById("book-download-link").value = book.download_link || "";
  document.getElementById("book-publication-year").value = book.publication_year || "";
  document.getElementById("book-page-count").value = book.page_count || "";
  document.getElementById("book-additional-info").value = book.additional_info || "";

  if (book.publisher_rel) {
    document.getElementById("book-publisher").value = book.publisher_rel.name || "";
    document.getElementById("book-publication-city").value = book.publisher_rel.city || "";
  } else {
    document.getElementById("book-publisher").value = "";
    document.getElementById("book-publication-city").value = "";
  }

  // Заполняем список авторов
  const container = document.getElementById("authors-container");
  container.innerHTML = "";
  if (book.authors && book.authors.length > 0) {
    book.authors.forEach(a => {
      const div = document.createElement("div");
      div.classList.add("author-entry");
      div.innerHTML = `
        <div class="input-group">
          <input type="text" class="author-last-name" placeholder="Last Name" value="${sanitizeAttribute(a.last_name)}" required>
        </div>
        <div class="input-group">
          <input type="text" class="author-first-name" placeholder="First Name" value="${sanitizeAttribute(a.first_name)}" required>
        </div>
        <div class="input-group">
          <input type="text" class="author-middle-name" placeholder="Middle Name" value="${sanitizeAttribute(a.middle_name)}">
        </div>
        <button type="button" class="remove-author-btn" onclick="removeAuthorField(this)">Remove</button>
      `;
      container.appendChild(div);
    });
  } else {
    addAuthorField();
  }

  clearAllErrors();
  showModal();
}

// Функция для удаления книги
async function deleteBook(bookId, silent=false) {
  if (!silent) {
    if (!confirm("Are you sure you want to delete this book?")) {
      return;
    }
  }

  const token = localStorage.getItem("token");
  if (!token) {
    showToast("Unauthorized", "error");
    return;
  }
  try {
    const resp = await fetch(`http://127.0.0.1:8000/books/${bookId}/`, {
      method: "DELETE",
      headers: { Authorization: `Bearer ${token}` },
    });
    if (!resp.ok) {
      throw new Error("Failed to delete book");
    }
    if (!silent) {
      showToast("Book deleted!", "success");
      await loadBooks();
    }
  } catch (error) {
    console.error(error);
    showToast("Error deleting book", "error");
  }
}

// Логика для добавления и удаления полей авторов
function addAuthorField() {
  const container = document.getElementById("authors-container");
  const div = document.createElement("div");
  div.classList.add("author-entry");
  div.innerHTML = `
    <div class="input-group">
      <input type="text" class="author-last-name" placeholder="Last Name" required>
    </div>
    <div class="input-group">
      <input type="text" class="author-first-name" placeholder="First Name" required>
    </div>
    <div class="input-group">
      <input type="text" class="author-middle-name" placeholder="Middle Name">
    </div>
    <button type="button" class="remove-author-btn" onclick="removeAuthorField(this)">Remove</button>
  `;
  container.appendChild(div);
  
  // Автоматическая прокрутка до нового поля
  div.scrollIntoView({ behavior: "smooth", block: "nearest" });
}

function removeAuthorField(btn) {
  const parent = btn.parentElement;
  if (parent && parent.classList.contains("author-entry")) {
    parent.remove();
  }
}

// Функция для получения списка авторов из формы
function getAuthors() {
  const entries = document.querySelectorAll(".author-entry");
  return Array.from(entries).map(en => {
    return {
      last_name: en.querySelector(".author-last-name").value.trim() || null,
      first_name: en.querySelector(".author-first-name").value.trim() || null,
      middle_name: en.querySelector(".author-middle-name").value.trim() || null,
    };
  }).filter(author => author.last_name || author.first_name || author.middle_name);
}

// Утилиты для очистки и отображения ошибок
function clearAllErrors() {
  const errs = document.querySelectorAll(".error-message");
  errs.forEach(e => e.textContent = "");
}

function showError(elementId, message) {
  const elem = document.getElementById(elementId);
  if (elem) {
    elem.textContent = message;
  }
}

// Функции для показа и скрытия модального окна
function showModal() {
  const modal = document.getElementById("book-modal");
  if (modal) {
    modal.classList.add("show");
  }
}

function hideModal() {
  const modal = document.getElementById("book-modal");
  if (modal) {
    modal.classList.remove("show");
  }
}

// Логика для отображения Toast уведомлений
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

// Функция для копирования текста в буфер обмена
function copyToClipboard(text) {
  navigator.clipboard.writeText(text).then(() => {
    showToast("Link copied to clipboard!", "success");
  }).catch(err => {
    console.error('Failed to copy: ', err);
    showToast("Failed to copy link", "error");
  });
}

// Функция выхода из системы
function logout() {
  localStorage.removeItem("token");
  localStorage.removeItem("username");
  localStorage.removeItem("role");
  window.location.href = "login.html";
}
