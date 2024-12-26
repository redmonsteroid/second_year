let editingBookId = null;  // Глобальная переменная для режима редактирования
let teacherBooks = [];     // Все книги учителя
let filteredBooks = [];    // Отфильтрованные по поиску (если есть)

// При загрузке проверяем роль, грузим книги
document.addEventListener("DOMContentLoaded", () => {
  const token = localStorage.getItem("token");
  const role = localStorage.getItem("role");
  if (!token || role !== "teacher") {
    alert("Access denied. You are not a teacher.");
    window.location.href = "../pages/library.html";
    return;
  }

  loadBooks();

  // Слушаем сабмит формы в модале
  const bookForm = document.getElementById("book-form");
  if (bookForm) {
    bookForm.addEventListener("submit", handleBookFormSubmit);
  }
});


function sanitizeOrNA(value) {
  const trimmed = value.trim();
  return trimmed === "" ? "N/A" : trimmed;
}

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

/* -------------- BOOK LOGIC -------------- */

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

    // По умолчанию нет фильтра поиска, так что покажем все
    filteredBooks = [...teacherBooks];
    renderBooks(filteredBooks);
  } catch (error) {
    console.error(error);
    showToast("Error loading books", "error");
  }
}

/**
 * Отображение книг в карточках
 */
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
        .map(a => `${a.last_name} ${a.first_name}${a.middle_name ? " " + a.middle_name : ""}`)
        .join(", ");
    }

    const card = document.createElement("div");
    card.classList.add("book-card");
    card.innerHTML = `
      <h4>${book.title || "N/A"}</h4>
      <p><strong>Authors:</strong> ${authorsText}</p>
      <p><strong>Publisher:</strong> ${publisherName}, <strong>City:</strong> ${publisherCity}</p>
      <p><strong>Year:</strong> ${book.publication_year ?? "N/A"}</p>
      <p><strong>Pages:</strong> ${book.page_count ?? "N/A"}</p>
      <p><strong>Link:</strong> ${
        book.download_link
          ? `<a href="${book.download_link}" target="_blank">${book.download_link}</a>`
          : "N/A"
      }</p>
      <p><strong>Info:</strong> ${book.additional_info || "N/A"}</p>
      <div class="book-actions">
        <button class="edit-btn" onclick="editBook(${book.id})">Edit</button>
        <button class="delete-btn" onclick="deleteBook(${book.id})">Delete</button>
      </div>
    `;
    bookCards.appendChild(card);
  });
}

/* ---------- SEARCH (filter) ---------- */
function searchTeacherBooks(searchVal) {
  const val = searchVal.trim().toLowerCase();
  filteredBooks = teacherBooks.filter(b => {
    // Проверяем title, authors, publisher, city, etc.
    const title = b.title ? b.title.toLowerCase() : "";
    const authors = b.authors
      ? b.authors.map(a => (a.last_name + " " + a.first_name + (a.middle_name||"")).toLowerCase()).join(" ")
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

/* ---------- ADD / EDIT BOOK (modal) ---------- */

/** 
 * Открыть модал для добавления новой книги 
 */
function openBookModal() {
  editingBookId = null;
  resetBookForm();
  document.getElementById("modal-title").textContent = "Add New Book";
  showModal();
}

/**
 * Закрыть модал
 */
function closeBookModal() {
  hideModal();
}

/**
 * Очистить форму
 */
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

/**
 * Обработчик сабмита формы (Add/Edit)
 */
/**
 * Обработчик сабмита формы (Add/Edit)
 */
async function handleBookFormSubmit(e) {
  e.preventDefault();
  clearAllErrors();

  const token = localStorage.getItem("token");
  if (!token) {
    showToast("Unauthorized", "error");
    return;
  }

  // Собираем данные из формы
  const titleVal = document.getElementById("book-title").value;
  const downloadVal = document.getElementById("book-download-link").value;
  const cityVal = document.getElementById("book-publication-city").value;
  const publisherVal = document.getElementById("book-publisher").value;
  const yearVal = document.getElementById("book-publication-year").value;
  const pageCountVal = document.getElementById("book-page-count").value;
  const additionalInfoVal = document.getElementById("book-additional-info").value;

  // Проверка обязательных полей
  const titleError = validateRequired(titleVal, "Book Title");
  const { year, error: yearError } = validatePublicationYear(yearVal);

  if (titleError) showError("book-title-error", titleError);
  if (yearError) showError("book-publication-year-error", yearError);

  // Если есть ошибки, прекращаем выполнение
  if (titleError || yearError) return;

  // Обработка авторов
  const authors = getAuthors();
  authors.forEach(author => {
    author.last_name = sanitizeOrNA(author.last_name);
    author.first_name = sanitizeOrNA(author.first_name);
  });

  // Парсим количество страниц
  const finalPageCount = pageCountVal.trim() !== "" ? parseInt(pageCountVal.trim(), 10) : null;

  // Формируем данные для отправки
  const newBookData = {
    title: titleVal.trim(),
    download_link: sanitizeOrNA(downloadVal),
    publication_year: year,
    page_count: finalPageCount,
    additional_info: sanitizeOrNA(additionalInfoVal),
    authors: authors,
    publisher: {
      name: sanitizeOrNA(publisherVal),
      city: sanitizeOrNA(cityVal),
    },
  };

  try {
    // Если редактирование
    if (editingBookId) {
      // Удаляем старую книгу
      await deleteBook(editingBookId, true);

      // Добавляем новую книгу
      const response = await fetch("http://127.0.0.1:8000/books/", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: `Bearer ${token}`,
        },
        body: JSON.stringify(newBookData),
      });

      if (!response.ok) {
        throw new Error("Failed to save book");
      }

      showToast("Book updated successfully!", "success");
    } else {
      // Если добавление новой книги
      const response = await fetch("http://127.0.0.1:8000/books/", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: `Bearer ${token}`,
        },
        body: JSON.stringify(newBookData),
      });

      if (!response.ok) {
        throw new Error("Failed to save book");
      }

      showToast("Book added successfully!", "success");
    }

    // Сбрасываем форму и перезагружаем книги
    document.getElementById("book-form").reset();
    editingBookId = null;
    await loadBooks();
    hideModal();
  } catch (error) {
    console.error(error);
    showToast("Error saving book", "error");
  }
}

/**
 * Функция editBook(bookId) - заполняет форму и показывает модал
 */
function editBook(bookId) {
  const book = teacherBooks.find(b => b.id === bookId);
  if (!book) {
    showToast("Book not found", "error");
    return;
  }

  document.getElementById("editing-book-id").value = bookId;
  document.getElementById("modal-title").textContent = "Edit Book";

  // Заполняем поля
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

  // Авторы
  const container = document.getElementById("authors-container");
  container.innerHTML = "";
  if (book.authors && book.authors.length > 0) {
    book.authors.forEach(a => {
      const div = document.createElement("div");
      div.classList.add("author-entry");
      div.innerHTML = `
        <div class="input-group">
          <input type="text" class="author-last-name" placeholder="Last Name" value="${a.last_name || ""}">
        </div>
        <div class="input-group">
          <input type="text" class="author-first-name" placeholder="First Name" value="${a.first_name || ""}">
        </div>
        <div class="input-group">
          <input type="text" class="author-middle-name" placeholder="Middle Name" value="${a.middle_name || ""}">
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

/* ---------------- Delete Book (silent) ---------------- */
async function deleteBook(bookId, silent=false) {
  const token = localStorage.getItem("token");
  if (!token) {
    showToast("Unauthorized", "error");
    return;
  }
  try {
    const resp = await fetch(`http://127.0.0.1:8000/books/${bookId}`, {
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

/* ---------------- Authors logic (Add / Remove fields) ---------------- */
function addAuthorField() {
  const container = document.getElementById("authors-container");
  const div = document.createElement("div");
  div.classList.add("author-entry");
  div.innerHTML = `
    <div class="input-group">
      <input type="text" class="author-last-name" placeholder="Last Name">
    </div>
    <div class="input-group">
      <input type="text" class="author-first-name" placeholder="First Name">
    </div>
    <div class="input-group">
      <input type="text" class="author-middle-name" placeholder="Middle Name">
    </div>
    <button type="button" class="remove-author-btn" onclick="removeAuthorField(this)">Remove</button>
  `;
  container.appendChild(div);
}

function removeAuthorField(btn) {
  btn.parentElement.remove();
}

function getAuthors() {
  const entries = document.querySelectorAll(".author-entry");
  return Array.from(entries).map(en => {
    return {
      last_name: en.querySelector(".author-last-name").value,
      first_name: en.querySelector(".author-first-name").value,
      middle_name: en.querySelector(".author-middle-name").value,
    };
  });
}

/* ---------------- Utility: Clear errors ---------------- */
function clearAllErrors() {
  const errs = document.querySelectorAll(".error-message");
  errs.forEach(e => e.textContent = "");
}

/* ---------------- Modal show/hide ---------------- */
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

/* ---------------- Toast logic ---------------- */
function showToast(msg, type="success") {
  const container = document.getElementById("toast-container");
  if (!container) return;
  const toast = document.createElement("div");
  toast.classList.add("toast");
  if (type === "error") {
    toast.style.backgroundColor = "#b52f2b";
  }
  toast.textContent = msg;
  
  container.appendChild(toast);

  // Удаляем через 3с
  setTimeout(() => {
    if (toast.parentNode) {
      toast.parentNode.removeChild(toast);
    }
  }, 3000);
}

/* ---------------- Logout ---------------- */
function logout() {
  localStorage.removeItem("token");
  localStorage.removeItem("username");
  localStorage.removeItem("role");
  window.location.href = "login.html";
}
