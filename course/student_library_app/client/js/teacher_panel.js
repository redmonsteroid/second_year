let editingBookId = null;  // Глобальная переменная для режима редактирования
let teacherBooks = [];     // Храним список книг учителя, чтобы найти нужную по ID

document.addEventListener("DOMContentLoaded", () => {
  // Проверка роли
  const token = localStorage.getItem("token");
  const role = localStorage.getItem("role");
  if (!token || role !== "teacher") {
    alert("Access denied. You are not a teacher.");
    window.location.href = "../pages/library.html";
    return;
  }

  console.log("Teacher panel loaded...");
  loadBooks();

  const addBookForm = document.getElementById("add-book-form");
  if (addBookForm) {
    addBookForm.addEventListener("submit", handleAddBook);
  }
});

/* ---------------- Валидация и вспомогательные функции ---------------- */
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

/* ---------------- Добавление / Редактирование книги ---------------- */

async function handleAddBook(event) {
  event.preventDefault();
  clearAllErrors();

  const token = localStorage.getItem("token");
  if (!token) {
    alert("Unauthorized. Please login.");
    return;
  }

  // Собираем значения из полей
  const titleVal = document.getElementById("book-title").value;
  const downloadVal = document.getElementById("book-download-link").value;
  const cityVal = document.getElementById("book-publication-city").value;
  const publisherVal = document.getElementById("book-publisher").value;
  const yearVal = document.getElementById("book-publication-year").value;
  const pageCountVal = document.getElementById("book-page-count").value;
  const additionalInfoVal = document.getElementById("book-additional-info").value;

  // Валидация
  const titleError = validateRequired(titleVal, "Book Title");
  if (titleError) {
    showError("book-title-error", titleError);
  }
  const { year, error: yearError } = validatePublicationYear(yearVal);
  if (yearError) {
    showError("book-publication-year-error", yearError);
  }

  if (titleError || yearError) {
    console.log("Validation failed. Title error:", titleError, "Year error:", yearError);
    return;
  }

  // Сбор остальных полей
  const finalDownloadLink = sanitizeOrNA(downloadVal);
  const finalCity = sanitizeOrNA(cityVal);
  const finalPublisher = sanitizeOrNA(publisherVal);
  const finalAdditionalInfo = sanitizeOrNA(additionalInfoVal);

  let finalPageCount = null;
  const pageTrim = pageCountVal.trim();
  if (pageTrim !== "") {
    const parsedPage = parseInt(pageTrim, 10);
    if (!isNaN(parsedPage) && parsedPage > 0) {
      finalPageCount = parsedPage;
    }
  }

  // Авторы
  const authors = getAuthors();
  authors.forEach((author) => {
    author.last_name = sanitizeOrNA(author.last_name);
    author.first_name = sanitizeOrNA(author.first_name);
    author.middle_name = sanitizeOrNA(author.middle_name);
  });

  // Формируем данные для POST /books
  const newBookData = {
    title: titleVal.trim(),
    download_link: finalDownloadLink,
    publication_year: year,  // либо null
    page_count: finalPageCount,
    additional_info: finalAdditionalInfo,
    authors,
    publisher: {
      name: finalPublisher,
      city: finalCity
    }
  };

  console.log("Submitting book data:", newBookData, "editingBookId:", editingBookId);

  try {
    // Если мы редактируем (editingBookId != null), удаляем старую книгу
    if (editingBookId !== null) {
      await deleteBook(editingBookId, /*silent=*/true);
      // silent=true означает, что мы не будем перезагружать список сразу
    }

    // Теперь создаём книгу
    const response = await fetch("http://127.0.0.1:8000/books/", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        Authorization: `Bearer ${token}`,
      },
      body: JSON.stringify(newBookData),
    });

    if (!response.ok) {
      throw new Error("Failed to add book");
    }

    console.log(editingBookId !== null
      ? "Book updated (delete + add) successfully!"
      : "Book added successfully!"
    );

    // Очищаем форму
    document.getElementById("add-book-form").reset();

    // Сброс режима редактирования
    editingBookId = null;
    const addBtn = document.querySelector(".add-book-btn");
    if (addBtn) {
      addBtn.textContent = "Add Book"; // Возвращаем текст кнопки
    }

    // Обновляем список
    await loadBooks();
  } catch (error) {
    console.error(error);
    alert("Error adding/updating book");
  }
}

/* ---------------- Функция editBook(bookId) ---------------- */
// Вызывается при нажатии на кнопку "Edit"
function editBook(bookId) {
  // Находим книгу в teacherBooks
  const book = teacherBooks.find(b => b.id === bookId);
  if (!book) {
    console.error("Book not found:", bookId);
    return;
  }

  console.log("Editing book:", book);

  // Заполняем поля формы
  document.getElementById("book-title").value = book.title || "";
  document.getElementById("book-download-link").value = book.download_link || "";
  document.getElementById("book-publication-year").value = book.publication_year || "";
  document.getElementById("book-page-count").value = book.page_count || "";
  document.getElementById("book-additional-info").value = book.additional_info || "";

  // Если есть publisher_rel => это означает, что на бэкенде есть publisher_rel.name/city
  if (book.publisher_rel) {
    document.getElementById("book-publisher").value = book.publisher_rel.name || "";
    document.getElementById("book-publication-city").value = book.publisher_rel.city || "";
  } else {
    document.getElementById("book-publisher").value = "";
    document.getElementById("book-publication-city").value = "";
  }

  // Авторы
  // Сначала очищаем блок authors-container
  const container = document.getElementById("authors-container");
  container.innerHTML = ""; // убираем все поля
  if (book.authors && book.authors.length > 0) {
    book.authors.forEach((a) => {
      const div = document.createElement("div");
      div.classList.add("author-entry");
      div.innerHTML = `
        <div class="input-group">
          <input type="text" class="author-last-name" placeholder="Last Name" value="${a.last_name || ""}">
          <span class="error-message author-error"></span>
        </div>
        <div class="input-group">
          <input type="text" class="author-first-name" placeholder="First Name" value="${a.first_name || ""}">
          <span class="error-message author-error"></span>
        </div>
        <div class="input-group">
          <input type="text" class="author-middle-name" placeholder="Middle Name" value="${a.middle_name || ""}">
          <span class="error-message author-error"></span>
        </div>
        <button type="button" class="remove-author-btn" onclick="removeAuthorField(this)">Remove</button>
      `;
      container.appendChild(div);
    });
  } else {
    // Если нет авторов, добавляем одну пустую запись
    addAuthorField();
  }

  // Включаем режим редактирования
  editingBookId = bookId;

  // Меняем текст кнопки на форме
  const addBtn = document.querySelector(".add-book-btn");
  if (addBtn) {
    addBtn.textContent = "Save Changes";
  }
}

/* ---------------- Очистка ошибок ---------------- */
function clearAllErrors() {
  const errorSpans = document.querySelectorAll(".error-message");
  errorSpans.forEach(span => {
    span.textContent = "";
  });
}

/* ---------------- Авторы ---------------- */
function addAuthorField() {
  const container = document.getElementById("authors-container");
  const div = document.createElement("div");
  div.classList.add("author-entry");
  div.innerHTML = `
    <div class="input-group">
      <input type="text" class="author-last-name" placeholder="Last Name">
      <span class="error-message author-error"></span>
    </div>
    <div class="input-group">
      <input type="text" class="author-first-name" placeholder="First Name">
      <span class="error-message author-error"></span>
    </div>
    <div class="input-group">
      <input type="text" class="author-middle-name" placeholder="Middle Name">
      <span class="error-message author-error"></span>
    </div>
    <button type="button" class="remove-author-btn" onclick="removeAuthorField(this)">Remove</button>
  `;
  container.appendChild(div);
}

function removeAuthorField(button) {
  button.parentElement.remove();
}

function getAuthors() {
  const authorEntries = document.querySelectorAll(".author-entry");
  return Array.from(authorEntries).map(entry => {
    return {
      last_name: entry.querySelector(".author-last-name").value,
      first_name: entry.querySelector(".author-first-name").value,
      middle_name: entry.querySelector(".author-middle-name").value,
    };
  });
}

/* ---------------- Загрузка и отображение книг (teacherBooks) ---------------- */

async function loadBooks() {
  const token = localStorage.getItem("token");
  if (!token) {
    alert("Unauthorized. Please login.");
    window.location.href = "login.html";
    return;
  }

  console.log("Loading books. Token found:", token);

  try {
    const response = await fetch("http://127.0.0.1:8000/books/", {
      headers: { Authorization: `Bearer ${token}` },
    });

    if (!response.ok) {
      throw new Error("Failed to load books");
    }

    const allBooks = await response.json();
    console.log("Books received from server:", allBooks);

    // Фильтруем только книги текущего учителя
    const currentUsername = localStorage.getItem("username");
    teacherBooks = allBooks.filter(book => book.owner_username === currentUsername);

    displayBooks(teacherBooks);
  } catch (error) {
    console.error(error);
    alert("Error loading books");
  }
}

/**
 * Выводим teacherBooks
 * Добавляем кнопку "Edit" наряду с кнопкой "Delete"
 */
function displayBooks(books) {
  const bookList = document.getElementById("book-list");
  if (!bookList) return;
  bookList.innerHTML = "";

  books.forEach((book) => {
    const publisherName = book.publisher_rel ? book.publisher_rel.name : "N/A";
    const publisherCity = book.publisher_rel ? book.publisher_rel.city : "N/A";

    let authorsText = "N/A";
    if (book.authors && book.authors.length > 0) {
      authorsText = book.authors
        .map(
          (a) => `${a.last_name} ${a.first_name}${a.middle_name ? " " + a.middle_name : ""}`
        )
        .join(", ");
    }

    const li = document.createElement("li");
    li.innerHTML = `
      <strong>Title:</strong> ${book.title || "N/A"} <br>
      <strong>Authors:</strong> ${authorsText} <br>
      <strong>Publisher:</strong> ${publisherName} <br>
      <strong>City:</strong> ${publisherCity} <br>
      <strong>Year:</strong> ${book.publication_year ?? "N/A"} <br>
      <strong>Pages:</strong> ${book.page_count ?? "N/A"} <br>
      <strong>Additional Info:</strong> ${book.additional_info || "N/A"} <br>
      <strong>Download Link:</strong> ${
        book.download_link
          ? `<a href="${book.download_link}" target="_blank">${book.download_link}</a>`
          : "N/A"
      } <br>

      <!-- Кнопки Edit и Delete -->
      <button class="edit-btn" onclick="editBook(${book.id})">Edit</button>
      <button class="delete-btn" onclick="deleteBook(${book.id})">Delete</button>
    `;
    bookList.appendChild(li);
  });
}

/* ---------------- Удаление книги (silent или нет) ---------------- */
async function deleteBook(bookId, silent = false) {
  const token = localStorage.getItem("token");
  if (!token) {
    alert("Unauthorized. Please login.");
    window.location.href = "login.html";
    return;
  }

  console.log("Deleting book with ID:", bookId);

  try {
    const response = await fetch(`http://127.0.0.1:8000/books/${bookId}`, {
      method: "DELETE",
      headers: { Authorization: `Bearer ${token}` },
    });

    if (!response.ok) {
      throw new Error("Failed to delete book");
    }

    console.log("Book deleted successfully:", bookId);

    if (!silent) {
      // Если не в режиме редактирования, перезагружаем список
      await loadBooks();
    }
  } catch (error) {
    console.error(error);
    alert("Error deleting book");
  }
}

/* ---------------- ЛОГОУТ ---------------- */
function logout() {
  localStorage.removeItem("token");
  localStorage.removeItem("username");
  localStorage.removeItem("role");
  console.log("Logged out. Redirecting to login...");
  window.location.href = "login.html";
}
