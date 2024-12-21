document.addEventListener("DOMContentLoaded", () => {
  console.log("Teacher panel loaded...");
  loadBooks();

  const addBookForm = document.getElementById("add-book-form");
  if (addBookForm) {
    addBookForm.addEventListener("submit", handleAddBook);
  }
});

/* ---------------- Валидация и вспомогательные функции ---------------- */

/**
 * Если строка состоит только из пробелов / пуста, заменяем на "N/A".
 */
function sanitizeOrNA(value) {
  const trimmed = value.trim();
  return trimmed === "" ? "N/A" : trimmed;
}

/**
 * Проверка обязательного поля (название книги)
 */
function validateRequired(value, fieldName) {
  const trimmed = value.trim();
  if (!trimmed) {
    return `Please provide a valid ${fieldName}. This field cannot be empty or only spaces.`;
  }
  return "";
}

/**
 * Проверка года публикации:
 * - Может быть пустым => null
 * - Если заполнен => 4 цифры, от 0 до 2025
 */
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

/* ---------------- Обработка формы добавления книги ---------------- */

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
  const cityVal = document.getElementById("book-publication-city").value;     // <-- город издательства
  const publisherVal = document.getElementById("book-publisher").value;      // <-- название издательства
  const yearVal = document.getElementById("book-publication-year").value;
  const pageCountVal = document.getElementById("book-page-count").value;
  const additionalInfoVal = document.getElementById("book-additional-info").value;

  // Проверяем обязательное поле: Title
  const titleError = validateRequired(titleVal, "Book Title");
  if (titleError) {
    showError("book-title-error", titleError);
  }

  // Проверяем год
  const { year, error: yearError } = validatePublicationYear(yearVal);
  if (yearError) {
    showError("book-publication-year-error", yearError);
  }

  // Остальные поля могут быть пусты => "N/A"
  const finalDownloadLink = sanitizeOrNA(downloadVal);
  const finalCity = sanitizeOrNA(cityVal);
  const finalPublisher = sanitizeOrNA(publisherVal);
  const finalAdditionalInfo = sanitizeOrNA(additionalInfoVal);

  // Парсим количество страниц (необязательно)
  let finalPageCount = null;
  const pageTrim = pageCountVal.trim();
  if (pageTrim !== "") {
    const parsedPage = parseInt(pageTrim, 10);
    if (!isNaN(parsedPage) && parsedPage > 0) {
      finalPageCount = parsedPage;
    }
  }

  // Обработка авторов
  const authors = getAuthors();
  authors.forEach((author) => {
    author.last_name = sanitizeOrNA(author.last_name);
    author.first_name = sanitizeOrNA(author.first_name);
    author.middle_name = sanitizeOrNA(author.middle_name);
  });

  // Если есть ошибки => прерываем
  if (titleError || yearError) {
    console.log("Validation failed. Title error:", titleError, "Year error:", yearError);
    return;
  }

  // Формируем объект для POST /books/
  // Обратите внимание: bэкенду теперь нужен publisher: { name, city },
  // а не просто publisher / publication_city
  const newBookData = {
    title: titleVal.trim(),
    download_link: finalDownloadLink,
    publication_year: year,            // либо null
    page_count: finalPageCount,
    additional_info: finalAdditionalInfo,
    authors: authors,
    publisher: {
      name: finalPublisher,
      city: finalCity
    }
  };

  console.log("Submitting new book with data:", newBookData);

  try {
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

    console.log("Book added successfully!");
    document.getElementById("add-book-form").reset();
    loadBooks();
  } catch (error) {
    console.error(error);
    alert("Error adding book");
  }
}

/* ---------------- Функции работы с ошибками ---------------- */

function clearAllErrors() {
  const errorSpans = document.querySelectorAll(".error-message");
  errorSpans.forEach(span => {
    span.textContent = "";
  });
}

function showError(spanId, message) {
  const span = document.getElementById(spanId);
  if (span) {
    span.textContent = message;
  }
}

/* ---------------- Логика с авторами ---------------- */

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

/* ---------------- Загрузка и отображение книг учителя ---------------- */

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

    const books = await response.json();
    console.log("Books received from server:", books);

    displayBooks(books);
  } catch (error) {
    console.error(error);
    alert("Error loading books");
  }
}

function displayBooks(allBooks) {
  const bookList = document.getElementById("book-list");
  if (!bookList) return;
  bookList.innerHTML = "";

  const currentUsername = localStorage.getItem("username");
  console.log("currentUsername from localStorage:", currentUsername);

  // Оставляем только книги текущего учителя
  const teacherBooks = allBooks.filter(book => {
    console.log(`Book ID: ${book.id}, Title: ${book.title}, OwnerUsername: ${book.owner_username}`);
    return book.owner_username === currentUsername;
  });

  console.log("Filtered teacherBooks:", teacherBooks);

  teacherBooks.forEach(book => {
    // Издательство и город теперь внутри book.publisher_rel
    const publisherName = book.publisher_rel ? book.publisher_rel.name : "N/A";
    const publisherCity = book.publisher_rel ? book.publisher_rel.city : "N/A";

    // Формируем строку с авторами
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
      <button class="delete-btn" onclick="deleteBook(${book.id})">Delete</button>
    `;
    bookList.appendChild(li);
  });
}

/* ---------------- Удаление книги (только своих) ---------------- */
async function deleteBook(bookId) {
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
    loadBooks();
  } catch (error) {
    console.error(error);
    alert("Error deleting book");
  }
}

/* ---------------- ЛОГОУТ ---------------- */
function logout() {
  localStorage.removeItem("token");
  localStorage.removeItem("username");
  console.log("Logged out. Redirecting to login...");
  window.location.href = "login.html";
}
